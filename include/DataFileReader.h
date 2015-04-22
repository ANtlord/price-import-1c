#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H
#include <fstream>
#include <vector>
#include <filter.h>
#include "FieldCoordinates.h"
#include <cassert>

class ReaderOptions
{
public:
    ReaderOptions(uint32_t startLine, uint32_t startCol, uint8_t numCol,
            uint8_t entryLines, bool isCascad)
    {
        assert(isCascad == (entryLines == 1));
        _startLine = startLine;
        _startCol = startCol;
        _numCol = numCol;
        _entryLines = entryLines;
        _isCascad = isCascad;
    }

    ReaderOptions(const ReaderOptions& value)
    {
        _startLine = value.getStartLine();
        _startCol = value.getStartCol();
        _numCol = value.getNumCol();
        _entryLines = value.getEntryLines();
        _isCascad = value.getIsCascad();
    }

    uint32_t getStartLine() const { return _startLine; }
    uint32_t getStartCol() const { return _startCol; }
    uint8_t getNumCol() const { return _numCol; }
    uint8_t getEntryLines() const { return _entryLines; }
    bool getIsCascad() const { return _isCascad; }

private:
    /// Number of first line, which contains information for import.
    uint32_t _startLine;
    /// Number of first column, which contains information for import.
    uint32_t _startCol;
    uint8_t _numCol;
    uint8_t _entryLines;
    bool _isCascad;
    /// If the flag will equal true, than program will get last entry from
    /// dupclicate data.
    //bool _isNeedLast;
};

class DataFileReader
{
public:
    DataFileReader(std::string filepath, const ReaderOptions& options,
            const FieldCoordinates &coordinates);
    virtual ~DataFileReader();
    virtual bool readline(std::string &buffer) = 0;
    virtual std::string * parseLine() = 0;
    const ReaderOptions &getOptions() const { return *_options; }
    const FieldCoordinates &getCoords() const { return *_coordinates; }

protected:
    //! Method copies elements from values to resValues.
    /*!
      Method copies elements from values to resValues. It prepare
      values for copying by filtering clear elements and elements, which are
      remain will be cleaned of front and back whitespaces. Therefore method
      works on data from aggregated price list, it has alogrithm based on state
      machine. And states are store in resValues and return value. It returns null
      when resValues are not ready, and returns them, when they are ready.
     */
    std::string * _setAggregatedValues();
    uint8_t _entryLineCounter;

    std::string _filepath;
    std::ifstream * _filestream;
    bool _isOpen;
    std::string _sectionName;

    // For parsing.
    std::string * _resValues;
    std::vector<std::string> _values;
    ReaderOptions * _options;

private:
    std::string * _incrementEntryLineCounter();
    const uint8_t _START_ENTRY_LINE = 1;
    void _setResValues(const uint8_t line);
    FieldCoordinates * _coordinates;
};

#endif //DATAFILEREADER_H
