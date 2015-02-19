#include "../include/ExcelReader.h"
#include <string.h>
#include <iostream>
using std::string;
using namespace xls;
ExcelReader::ExcelReader(std::string filepath) : DataFileReader(filepath)
                                                 , activeWorkSheet(nullptr)
{
    string encoding = "utf-8";
    workBook=xls::xls_open(_filepath.c_str(), encoding.c_str());
    if(workBook) {
		numSheets = workBook->sheets.count;
		xls_parseWorkBook(workBook);
	}
    else {
		throw std::string("failed to open the requested file!");
	}
    uint32_t workSheetIndex = 0;
    uint16_t row = 0;
    uint16_t col = 0;
    uint32_t sheetNum = 0;
    activeWorkSheet = xls::xls_getWorkSheet(workBook, sheetNum);

    cellContent content;
	
	//assert(row && col);

	InitIterator();
	
	OpenSheet(workSheetIndex);
	
	//--row, --col;
	
	uint32_t numRows = activeWorkSheet->rows.lastrow + 1;
	uint32_t numCols = activeWorkSheet->rows.lastcol + 1;

	for (uint32_t t=0; t<numRows; t++)
	{
        xlsRow *rowP = &activeWorkSheet->rows.row[t];
		for (uint32_t tt=0; tt<numCols; tt++)
		{
			xlsCell	*cell = &rowP->cells.cell[tt];
			if(cell->row < row) break;
            if(cell->row > row) {
                std::cout << content.row << std::endl;
                tt=numCols;
                t=numRows;
            } 
			
			if(cell->id == 0x201) continue;
			
			if(cell->col == col) {
				FormatCell(cell, content);
				//return content;
                std::cout << content.row << std::endl;
                tt=numCols;
                t=numRows;
			}
		}
	}
}

void ExcelReader::FormatCell(xlsCell *cell, cellContent& content) const
{
	uint32_t col = cell->col;

	content.str = char2string(cell->str);
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
		} else {
			if(!strcmp((char *)cell->str, "bool")) {
				content.type = cellBool;
				content.val.b = (bool)cell->d;
			} else
			if(!strcmp((char *)cell->str, "error")) {
				content.type = cellError;
				content.val.e = (int32_t)cell->d;
			} else {
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

std::string ExcelReader::char2string(const uint8_t *ptr) const
{
	return string((const char *)ptr);
}

void ExcelReader::OpenSheet(uint32_t sheetNum)
{	
	if(sheetNum >= numSheets) {
		throw string("no such sheet exists!");
	}
    else if(sheetNum != activeWorkSheetID) {
		activeWorkSheetID = sheetNum;
		xls_close_WS(activeWorkSheet);
		activeWorkSheet = xls_getWorkSheet(workBook, sheetNum);
		xls_parseWorkSheet(activeWorkSheet);
	}
}

void ExcelReader::InitIterator(uint32_t sheetNum)
{
	if(sheetNum != UINT32_MAX) {
		OpenSheet(sheetNum);
		iterating = true;
		lastColIndex = 0;
		lastRowIndex = 0;
	} else {
		iterating = false;
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

}
