#include "../include/DataFileReader.h"
#include <split.h>
#include <trim.h>
#include <filter.h>
#include <each.h>

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

std::string * DataFileReader::_setAggregatedValues(std::vector<std::string>& values,
        std::string * resValues)
{
    values = forge::filter<std::string>(
        [](const std::string& item) -> bool {return item.size() > 0;}, values
    );
    forge::each<std::string>(
        [](std::string& item) -> void {forge::trim(item);}, values
    );

    if (values.size() == 1) {
        // If programm gets 1 col twice, then it was name of section.
        if (resValues[0] != "") {
            _sectionName = resValues[0];
        } 
        // If it is not section's name, it is product's name. Code and
        // price of it will be added in next iteration.
        resValues[0] = values[0];
        return nullptr;
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
    return nullptr;
}
