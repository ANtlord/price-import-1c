#include "../include/ExcelReader.h"
#include <xls.h>
#include <string.h>
using std::string;
ExcelReader::ExcelReader(std::string filepath) : DataFileReader(filepath)
{

}

bool ExcelReader::readline(std::string &buffer)
{
    xls::xlsWorkBook* pWB;
    xls::xlsWorkSheet* pWS;
    string encoding = "utf-8";
    pWB=xls::xls_open(_filepath.c_str(), encoding.c_str());
 
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
