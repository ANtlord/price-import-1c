#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H
#include <fstream>
#include <vector>

class ReaderOptions
{
public:
    ReaderOptions(uint32_t startLine, uint32_t startCol, uint8_t numCol,
            uint8_t entryLines, bool isCascad)
    {
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
    uint32_t _startLine;
    uint32_t _startCol;
    uint8_t _numCol;
    uint8_t _entryLines;
    bool _isCascad;
};

class DataFileReader
{
public:
    DataFileReader(std::string filepath, const ReaderOptions& options);
    virtual ~DataFileReader();
    virtual bool readline(std::string &buffer) = 0;
    virtual std::string * parseLine() = 0;
    const ReaderOptions &getOptions() const { return *_options; }

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

    std::string _filepath;
    std::ifstream * _filestream;
    bool _isOpen;
    std::string _sectionName;

    // For parsing.
    std::string * _resValues;
    std::vector<std::string> _values;
    ReaderOptions * _options;
};

#endif //DATAFILEREADER_H
