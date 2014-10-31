#include "../include/csvreader.h"

CSVreader::CSVreader(std::string filepath)
{
    _filepath = filepath;
    _isOpen = false;
    _filestream = 0;
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
    }
}

CSVreader::~CSVreader()
{
    if (_filestream != 0 && _filestream->is_open()) {
        _filestream->close();
        delete _filestream;
    }
}
