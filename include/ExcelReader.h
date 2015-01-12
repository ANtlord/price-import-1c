#ifndef EXCELREADER_H
#define EXCELREADER_H
#include "DataFileReader.h"

class ExcelReader : public DataFileReader
{
public:
    ExcelReader(std::string filepath);
    virtual bool readline(std::string &buffer);
    virtual std::string * parseLine();
    virtual ~ExcelReader();

private:
    /* data */
};

#endif //EXCELREADER_H
