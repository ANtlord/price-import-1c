#ifndef CSVREADER_H
#define CSVREADER_H

#include <string.h>
#include <fstream>
#include "DataFileReader.h"

class CSVreader : public DataFileReader
{
public:
    CSVreader(std::string filepath, const ReaderOptions& options);
    virtual ~CSVreader();
    bool readline(std::string &buffer) override;
    std::string * parseLine() override;

private:
    uint16_t _lineCounter = 0;

};

#endif //CSVREADER_H
