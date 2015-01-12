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
            } 
            // If it is not section's name, it is product's name. Code and
            // price of it will be added in next iteration.
            resValues[0] = values[0];   
        }
        else if (values.size() == 2 && resValues[0] != "") {
            resValues[1] = values[0];

            std::replace(values[1].begin(), values[1].end(), ',', '.');
            forge::filter([](char &c) -> bool {return (c=='.' || c>47 && c<58);
                    }, values[1]);  // all numbers and dot must be right. [0-9]\.

            resValues[2] = values[1];
            resValues[3] = _sectionName;
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
