#include "../include/csvreader.h"
#include <split.h>
#include <trim.h>
#include <filter.h>
#include <each.h>

CSVreader::CSVreader(std::string filepath)
{
    i=0;
    _filepath = filepath;
    _isOpen = false;
    _filestream = 0;
    _sectionName = "";
}

std::string * CSVreader::parseLine()
{
    std::string * result = 0;
    std::string line;
    std::vector<std::string> values;

    std::string * resValues = new std::string[4];
    while(readline(line)){
        values = forge::split(line, ';');
        values = forge::filter<std::string>(
            [](const std::string& item) -> bool {return item.size() > 0;},
            values
        );
        forge::each<std::string>(
            [](std::string& item) -> void {forge::trim(item);}, values
        );

        if (values.size() == 1) {
            // If twice programm gets 1 col, then it was name of section.
            if (resValues[0] != "") {
                _sectionName = resValues[0];
                if (i==2) {
                    std::cout << _sectionName << std::endl;
                }
            } 
            resValues[0] = values[0];
        }
        else if (values.size() == 2 && resValues[0] != "") {
            resValues[1] = values[0];

            std::replace(values[1].begin(), values[1].end(), ',', '.');
            size_t n = 0;
            while ((n = values[1].find(' ')) != std::string::npos) {
                values[1].erase(n);
                if (values[1] == "3 348.00") {
                    std::cout << values[1] << std::endl;
                }
            }

            resValues[2] = values[1];
            resValues[3] = _sectionName;
            if (i==2) {
                std::cout << _sectionName << std::endl;
            }
            return resValues;
        }
    }
    _sectionName = "";
    return 0;
}

bool CSVreader::readline(std::string &buffer){
    if (!_isOpen) {
        _filestream = new std::ifstream(_filepath);
        _isOpen = true;
        ++i;
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
    if (_filestream != 0 && _filestream->is_open()) {
        _filestream->close();
        delete _filestream;
    }
}
