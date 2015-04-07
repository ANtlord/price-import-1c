#include "../include/csvreader.h"
#include <split.h>
#include <trim.h>
#include <filter.h>
#include <each.h>

using std::string;
using std::vector;

CSVreader::CSVreader(std::string filepath, const ReaderOptions& options,
        const FieldCoordinates &coordinates) :
    DataFileReader(filepath, options, coordinates)
{
}

string * CSVreader::parseLine()
{
    string line;
    _resValues = new string[(&getOptions())->getNumCol()];

    while(readline(line)){
        if (_skipLineCounter >= (&getOptions())->getStartLine()) {
            _values = forge::split(line, ';');

            auto res = _setAggregatedValues(); 

            // If result is not equal null, it means that it store information
            // from entry block.
            if (res != nullptr) {
                return res; 
            }
        }
        else {
           ++_skipLineCounter;
        }
    }
    _skipLineCounter = 0;
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
