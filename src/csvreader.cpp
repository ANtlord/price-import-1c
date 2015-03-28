#include "../include/csvreader.h"
#include <split.h>
#include <trim.h>
#include <filter.h>
#include <each.h>

CSVreader::CSVreader(std::string filepath, const ReaderOptions& options) :
    DataFileReader(filepath, options)
{
}

std::string * CSVreader::parseLine()
{
    std::string line;
    _resValues = new std::string[getOptions().getNumCol()];

    while(readline(line)){
        if (_lineCounter >= getOptions().getStartLine()) {
            _values = forge::split(line, ';');
            auto res = _setAggregatedValues(); 
            if (res != nullptr) {
                return res; 
            }
        }
        else {
           ++_lineCounter;
        }
    }
    _lineCounter = 0;
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
