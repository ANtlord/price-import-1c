#include "../include/DataFileReader.h"
#include <split.h>
#include <trim.h>
#include <filter.h>
#include <each.h>

DataFileReader::DataFileReader(std::string filepath, const ReaderOptions& options)
{
    _filepath = filepath;
    _isOpen = false;
    _filestream = nullptr;
    _sectionName = "";
    _resValues = nullptr;
    _options = new ReaderOptions(options);
}

DataFileReader::~DataFileReader()
{
    if (_filestream != nullptr && _filestream->is_open()) {
        _filestream->close();
        delete _filestream;
    }
    delete _options;
}

std::string * DataFileReader::_setAggregatedValues()
{
    forge::each<std::string>(
        [](std::string& item) -> void {forge::trim(item);}, _values
    );
    _values = forge::filter<std::string>(
        [](const std::string& item) -> bool {return item.size() > 0;}, _values
    );

    if (_values.size() == 1) {
        // If programm gets 1 col twice, then it was name of section.
        if (_resValues[0] != "") {
            _sectionName = _resValues[0];
        } 
        // If it is not section's name, it is product's name. Code and
        // price of it will be added in next iteration.
        _resValues[0] = _values[0];
        return nullptr;
    }
    else if (_values.size() == 2 && _resValues[0] != "") {
        _resValues[1] = _values[0];

        std::replace(_values[1].begin(), _values[1].end(), ',', '.');
        forge::filter([](char &c) -> bool {return (c=='.' || c>47 && c<58);
                }, _values[1]);  // all numbers and dot must be right. [0-9]\.

        _resValues[2] = _values[1];
        _resValues[3] = _sectionName;
        return _resValues;
    }
    return nullptr;
}
