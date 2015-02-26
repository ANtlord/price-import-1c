#ifndef EXCELREADER_H
#define EXCELREADER_H
#include "DataFileReader.h"
#include <xls.h>

enum ContentsType : uint8_t { cellBlank=0, cellString, cellInteger, cellFloat,
    cellBool, cellError, cellUnknown };

class CellContent {
public:
    CellContent() :
        type(cellBlank),
        colStr(),
        col(0),
        row(0),
        val(0) { }
    ~CellContent() {}
    ContentsType		type;
    char				colStr[3];	// String "A"..."Z", "AA"..."ZZ" (second char is either nil or a capital letter)
    uint32_t			col;		// 1 based
    uint16_t			row;		// 1 based
    std::string			str;		// even for numbers these values are formatted as well as provided below
    union Val {
        Val(int x) { l = x; }
        ~Val() { }

        long			l;
        double			d;
        bool			b;
        int32_t			e;
    } val;
};

class ExcelReader : public DataFileReader
{
public:
    ExcelReader(std::string filepath);
    bool readline(std::string &buffer) override;
    std::string * parseLine() override;
    virtual ~ExcelReader();

private:
    void _openSheet(uint32_t sheetNum);
    void _initIterator(uint32_t sheetNum = UINT32_MAX);  // call this first...
    std::string _char2string(const uint8_t *ptr) const;

    xls::xlsWorkSheet * _activeWorkSheet; // keep last one active
    bool _isIterating;
    void _formatCell(xls::xlsCell *cell, CellContent& content) const;
    uint32_t _lastRowIndex;
    uint32_t _lastColIndex;
    uint32_t _activeWorkSheetID;		// keep last one active
    xls::xlsWorkBook* _workBook;
    uint32_t _numSheets = 0;
};

#endif //EXCELREADER_H
