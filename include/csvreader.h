#ifndef CSVREADER_H
#define CSVREADER_H

#include <string.h>
#include <fstream>
#include "DataFileReader.h"

class CSVreader : public DataFileReader
{
public:
    CSVreader(std::string filepath, const ReaderOptions& options,
            const FieldCoordinates &coordinates);
    virtual ~CSVreader();
    bool readline(std::string &buffer) override;
    std::string * parseLine() override;

private:
    uint16_t _skipLineCounter = 0;

};

#endif //CSVREADER_H
