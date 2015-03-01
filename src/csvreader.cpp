#include "../include/csvreader.h"
#include <split.h>
#include <trim.h>
#include <filter.h>
#include <each.h>

CSVreader::CSVreader(std::string filepath) : DataFileReader(filepath)
{
}

std::string * CSVreader::parseLine()
{
    std::string line;
    _resValues = new std::string[4];

    while(readline(line)){
        _values = forge::split(line, ';');
        auto res = _setAggregatedValues(); 
        if (res != nullptr) {
            return res; 
        }
    }
    _sectionName = "";
    return nullptr;
}

bool CSVreader::readline(std::string &buffer){
    if (!_isOpen) {
        _filestream = new std::ifstream(_filepath);
        _isOpen = true;
    }

    if (_filestream->eof() || _filestream->bad()) {
        _filestream->close();
        _isOpen = false;
        return false;
    }
    else {
        while (getline(*_filestream, buffer)){
            return true;
        }
        _filestream->close();
        _isOpen = false;
        return false;
    }
}

CSVreader::~CSVreader()
{
}
