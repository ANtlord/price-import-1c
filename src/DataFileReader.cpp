#include "../include/DataFileReader.h"
#include <split.h>
#include <trim.h>
#include <filter.h>
#include <each.h>

DataFileReader::DataFileReader(std::string filepath,
        const ReaderOptions& options,
        const FieldCoordinates &coordinates)
{
    _filepath = filepath;
    _isOpen = false;
    _filestream = nullptr;
    _sectionName = "";
    _resValues = nullptr;
    _options = new ReaderOptions(options);
    _coordinates = new FieldCoordinates(coordinates);
    _entryLineCounter = _START_ENTRY_LINE;
}

DataFileReader::~DataFileReader()
{
    if (_filestream != nullptr) {
        if (_filestream->is_open()) _filestream->close();
        delete _filestream;
    }
    delete _options;
    std::cout << "FUCK!!!" << std::endl;
    delete _coordinates;
}

std::string * DataFileReader::_incrementEntryLineCounter()
{
    if (_entryLineCounter == getOptions().getEntryLines() - 1){  // Length - 1.
        _entryLineCounter = _START_ENTRY_LINE;
        return _resValues;
    }
    else {
        ++_entryLineCounter;
        return nullptr;
    }
}

void normalizePriceValue(std::string &value)
{
    std::replace(value.begin(), value.end(), ',', '.');
    forge::filter([](char &c) -> bool {
        return (c=='.' || c>47 && c<58);
    }, value);  // all numbers and dot must be correct. [0-9].
}

void DataFileReader::_setResValues(const uint8_t line)
{
    const FieldCoordinates * COORDS = &getCoords();
    for (uint8_t i = 0; i < _values.size(); ++i) {
        if (COORDS->isPrice(i, line)) normalizePriceValue(_values[i]);

        try {
            _resValues[COORDS->getResultIndex(i, line)] = _values[i];
        } catch (ResultIndexesException &e){
            printf("%s\n", e.what(i, line));
        }
    }
}

std::string * DataFileReader::_setAggregatedValues()
{
    const uint8_t SECTION_LINE = 0;
    const uint8_t PREV_LINE = _entryLineCounter;
    const bool IS_NOT_EMPTY_NAME = _resValues[ResultIndexes::NAME] != "";

    forge::each<std::string>(
        [](std::string& item) -> void {forge::trim(item);}, _values
    );
    _values = forge::filter<std::string>(
        [](const std::string& item) -> bool {return item.size() > 0;}, _values
    );
    const uint8_t SECTION_LINE_SIZE = getCoords().getLineWidth(SECTION_LINE);
    const bool IS_SECTION_LINE_SIZE = (SECTION_LINE_SIZE == _values.size());


    // I suppose, that section name is on first line always. It should work
    // for cascade price lists.
    if (IS_SECTION_LINE_SIZE) {
        // If programm gets single column twice and the cell of this column is
        // on 2nd line of entry block, then it was name of section.
        if (IS_NOT_EMPTY_NAME && PREV_LINE == (_START_ENTRY_LINE + 1)) {
            _sectionName = _resValues[ResultIndexes::NAME];
            --_entryLineCounter;
        }

        // If number of columns in first line of an entry block equals
        // number of section's line columns.
        if (getCoords().getLineWidth(_START_ENTRY_LINE) == SECTION_LINE_SIZE)
            _setResValues(_entryLineCounter);
        return _incrementEntryLineCounter();
    }
    else if (!IS_SECTION_LINE_SIZE && IS_NOT_EMPTY_NAME) {
        _setResValues(PREV_LINE);
        _resValues[ResultIndexes::SECTION] = _sectionName;
        return _incrementEntryLineCounter();
    }
    else {
        // Here program gets values of duplicated codes and prices.
        // TODO: handle this
    }

    return nullptr;
}
