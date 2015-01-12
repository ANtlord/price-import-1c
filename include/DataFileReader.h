#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H
#include <fstream>

class DataFileReader
{
public:
    DataFileReader(std::string filepath);
    virtual ~DataFileReader();
    virtual bool readline(std::string &buffer) = 0;
    virtual std::string * parseLine() = 0;

protected:
    std::string _filepath;
    std::ifstream * _filestream;
    bool _isOpen;
    std::string _sectionName;
};

#endif //DATAFILEREADER_H
