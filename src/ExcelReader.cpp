#include "../include/ExcelReader.h"
#include <libxls/xls.h>
using std::string;
ExcelReader::ExcelReader(std::string filepath) : DataFileReader(filepath)
{

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
