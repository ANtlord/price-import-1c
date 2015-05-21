#include "../include/ExcelReader.h"
#include <string.h>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using namespace xls;

ExcelReader::ExcelReader(std::string filepath, const ReaderOptions& options,
        const FieldCoordinates &coordinates) :
    DataFileReader(filepath, options, coordinates),
    _activeWorkSheet(nullptr),
	_activeWorkSheetID(-1),
	_workBook(nullptr),
	_numSheets(0)
{
    string encoding = "utf-8";
    _workBook = xls_open(_filepath.c_str(), encoding.c_str());
    if(_workBook) {
		_numSheets = _workBook->sheets.count;
		xls_parseWorkBook(_workBook);
	}
    else {
		throw std::string("failed to open the requested file!");
	}
    uint32_t workSheetIndex = 0;
    uint32_t sheetNum = 0;
    _activeWorkSheet = xls::xls_getWorkSheet(_workBook, sheetNum);
    xls::xls_parseWorkSheet(_activeWorkSheet);

	_initIterator();
	_openSheet(workSheetIndex);
}

void ExcelReader::_formatCell(xlsCell *cell, CellContent& content) const
{
	uint32_t col = cell->col;

	content.str = _char2string(cell->str);
	content.row = cell->row + 1;
	
	content.col = col + 1;
	if(col < 26) {
		content.colStr[0] = 'A' + (char)col;
		content.colStr[1] = '\0';
	} else {
		content.colStr[0] = 'A' + (char)(col/26);
		content.colStr[1] = 'A' + (char)(col%26);
	}
	content.colStr[2] = '\0';

	switch(cell->id) {
    case 0x0006:	//FORMULA
		// test for formula, if
        if(cell->l == 0) {
			content.type = cellFloat;
			content.val.d = cell->d;
		}
        else {
			if(!strcmp((char *)cell->str, "bool")) {
				content.type = cellBool;
				content.val.b = (bool)cell->d;
			}
            else if(!strcmp((char *)cell->str, "error")) {
				content.type = cellError;
				content.val.e = (int32_t)cell->d;
			}
            else {
				content.type = cellString;
			}
		}
        break;
    case 0x00FD:	//LABELSST
    case 0x0204:	//LABEL
		content.type = cellString;
		content.val.l = cell->l;	// possible numeric conversion done for you
		break;
    case 0x0203:	//NUMBER
    case 0x027E: {	//RK
        // TODO: if you will meet issue which will be related with numbers
        // a.00 and a, and they should be different, try to fix it in libxls
        // xls.c xls_addCell(xlsWorkSheet* pWS,BOF* bof,BYTE* buf)
        const auto n = content.str.find('.');
        if (n != string::npos) {
            auto counter = n;
            for (size_t i = n+1; i < content.str.length(); ++i)
                if (content.str.at(i) != '0') counter = i+1;
            content.str.erase(counter);
        }
		content.type = cellFloat;
		content.val.d = cell->d;
        break;
    }
    default:
		content.type = cellUnknown;
        break;
    }
}

std::string ExcelReader::_char2string(const uint8_t *ptr) const
{
	return string((const char *)ptr);
}

void ExcelReader::_openSheet(uint32_t sheetNum)
{	
	if(sheetNum >= _numSheets) {
		throw string("no such sheet exists!");
	}
    else if(sheetNum != _activeWorkSheetID) {
		_activeWorkSheetID = sheetNum;
		xls_close_WS(_activeWorkSheet);
		_activeWorkSheet = xls_getWorkSheet(_workBook, sheetNum);
		xls_parseWorkSheet(_activeWorkSheet);
	}
}

void ExcelReader::_initIterator(uint32_t sheetNum)
{
	if(sheetNum != UINT32_MAX) {
		_openSheet(sheetNum);
		_isIterating = true;
		_lastColIndex = 0;
	}
    else {
		_isIterating = false;
	}
    _lastRowIndex = getOptions().getStartLine();
}

bool ExcelReader::readline(std::string &buffer)
{
    return false;
}

std::string * ExcelReader::parseLine()
{
	CellContent content;
	uint32_t numRows = _activeWorkSheet->rows.lastrow + 1;
    _resValues = new std::string[getOptions().getNumCol()];

	for (uint32_t t=_lastRowIndex; t<numRows; t++) {
		xlsRow *rowP = &_activeWorkSheet->rows.row[t];

        _values.clear();
		for (uint32_t tt=rowP->fcell; tt<rowP->lcell; tt++) {
			xlsCell	*cell = &rowP->cells.cell[tt];

            if(cell->id != 0x201 && cell->isHidden == 0) {
                _formatCell(cell, content);

                _values.push_back(content.str);
            }
        }
        auto res = _setAggregatedValues(); 
        if (res != nullptr) {
            return res; 
        }
        ++_lastRowIndex;
    }
    _lastRowIndex = getOptions().getStartLine();
    _sectionName = "";
    return nullptr;
}

ExcelReader::~ExcelReader()
{
	xls_close_WS(_activeWorkSheet);
	xls_close_WB(_workBook);	// handles nil parameter
}
