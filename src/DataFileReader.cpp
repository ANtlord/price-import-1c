#include "../include/DataFileReader.h"

DataFileReader::DataFileReader(std::string filepath)
{
    _filepath = filepath;
    _isOpen = false;
    _filestream = 0;
    _sectionName = "";
}

DataFileReader::~DataFileReader()
{
    if (_filestream != 0 && _filestream->is_open()) {
        _filestream->close();
        delete _filestream;
    }
}
