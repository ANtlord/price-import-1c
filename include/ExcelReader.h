#ifndef EXCELREADER_H
#define EXCELREADER_H
#include "DataFileReader.h"
#include <xls.h>

typedef enum { cellBlank=0, cellString, cellInteger, cellFloat, cellBool, cellError, cellUnknown } contentsType;
struct cellContent {
    contentsType		type;
    char				colStr[3];	// String "A"..."Z", "AA"..."ZZ" (second char is either nil or a capital letter)
    uint32_t			col;		// 1 based
    uint16_t			row;		// 1 based
    std::string			str;		// even for numbers these values are formatted as well as provided below
    union Val {
        long			l;
        double			d;
        bool			b;
        int32_t			e;
        
    Val(int x) { l = x; }
    ~Val() { }
    } val;
    
    cellContent(void) :
        type(cellBlank),
        colStr(),
        col(0),
        row(0),
        val(0) { }
    ~cellContent() {}
};

class ExcelReader : public DataFileReader
{
public:
    ExcelReader(std::string filepath);
    bool readline(std::string &buffer) override;
    std::string * parseLine() override;
    virtual ~ExcelReader();

    // temp fields.
    void FormatCell(xls::xlsCell *cell, cellContent& content) const;
    void InitIterator(uint32_t sheetNum = UINT32_MAX);  // call this first...
    void OpenSheet(uint32_t sheetNum);
    xls::xlsWorkSheet * activeWorkSheet; // keep last one active
    uint32_t numSheets = 0;
    std::string char2string(const uint8_t *ptr) const;
    uint32_t activeWorkSheetID;		// keep last one active
    xls::xlsWorkBook* workBook;
    bool iterating;
    uint32_t			lastRowIndex;
    uint32_t			lastColIndex;

private:
};

#endif //EXCELREADER_H
