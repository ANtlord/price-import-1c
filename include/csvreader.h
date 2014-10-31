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

private:
    std::string _filepath;
    std::ifstream * _filestream;
    bool _isOpen;
};

#endif //CSVREADER_H
