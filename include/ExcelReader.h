#ifndef EXCELREADER_H
#define EXCELREADER_H
#include "DataFileReader.h"

class ExcelReader : public DataFileReader
{
public:
    ExcelReader(std::string filepath);
    bool readline(std::string &buffer) override;
    std::string * parseLine() override;
    virtual ~ExcelReader();

private:
};

#endif //EXCELREADER_H
