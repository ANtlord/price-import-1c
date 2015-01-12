#ifndef CSVREADER_H
#define CSVREADER_H

#include <string.h>
#include <fstream>
#include "DataFileReader.h"

class CSVreader : public DataFileReader
{
public:
    CSVreader(std::string filepath);
    virtual ~CSVreader();
    bool readline(std::string &buffer) override;
    std::string * parseLine() override;

private:
};

#endif //CSVREADER_H
