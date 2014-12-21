#ifndef CSVREADER_H
#define CSVREADER_H

#include <string.h>
#include <fstream>

class CSVreader
{
public:
    CSVreader(std::string filepath);
    virtual ~CSVreader();
    bool readline(std::string &buffer);
    std::string * parseLine();

private:
    std::string _filepath;
    std::ifstream * _filestream;
    bool _isOpen;
    std::string _sectionName;
    int i;
};

#endif //CSVREADER_H
