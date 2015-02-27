#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H
#include <fstream>
#include <vector>

class DataFileReader
{
public:
    DataFileReader(std::string filepath);
    virtual ~DataFileReader();
    virtual bool readline(std::string &buffer) = 0;
    virtual std::string * parseLine() = 0;

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
};

#endif //DATAFILEREADER_H
