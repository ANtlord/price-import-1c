#include "../include/ExcelReader.h"
#include <string.h>
#include <iostream>
using std::string;
using namespace xls;
ExcelReader::ExcelReader(std::string filepath) : DataFileReader(filepath),
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
    uint16_t row = 0;
    uint16_t col = 0;
    uint32_t sheetNum = 0;
    _activeWorkSheet = xls::xls_getWorkSheet(_workBook, sheetNum);
    xls::xls_parseWorkSheet(_activeWorkSheet);

    CellContent content;
	
	//assert(row && col);

	_initIterator();
	
	_openSheet(workSheetIndex);
	
    --row, --col;
	
	uint32_t numRows = _activeWorkSheet->rows.lastrow + 1;
	uint32_t numCols = _activeWorkSheet->rows.lastcol + 1;

    std::cout << numRows << std::endl;
    std::cout << numCols << std::endl;
    std::cout << "------------<!: " << std::endl;

    xlsRow *rowP = &_activeWorkSheet->rows.row[1];
    xlsCell	*cell = &rowP->cells.cell[0];
    _formatCell(cell, content);
    std::cout << content.val.d << std::endl;
    //for (uint32_t t=0; t<numRows; t++) {
        //xlsRow *rowP = &_activeWorkSheet->rows.row[t];
        //for (uint32_t tt=0; tt<numCols; tt++){
            //xlsCell	*cell = &rowP->cells.cell[tt];
            //if(cell->row < row) break;
            //if(cell->row > row) {
                //std::cout << content.str << std::endl;
                //tt=numCols;
                //t=numRows;
            //} 
            
            //if(cell->id == 0x201) continue;
            
            //if(cell->col == col) {
                //_formatCell(cell, content);
                //std::cout << content.str << std::endl;
                ////return content;
                //tt=numCols;
                //t=numRows;
            //}
        //}
    //}
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
    case 0x027E:	//RK
		content.type = cellFloat;
		content.val.d = cell->d;
        break;
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
		_lastRowIndex = 0;
	}
    else {
		_isIterating = false;
	}
}

bool ExcelReader::readline(std::string &buffer)
{
    return false;
}

std::string * ExcelReader::parseLine()
{
    string * qwe = new string();
    return qwe;
}

ExcelReader::~ExcelReader()
{
	xls_close_WS(_activeWorkSheet);
	xls_close_WB(_workBook);	// handles nil parameter
}
