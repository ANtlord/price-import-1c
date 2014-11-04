#include <iostream>
#include "include/csvreader.h"
#include <cassert>

int main(int argc, char *argv[])
{
    assert(argc > 1);   // Specifying filepath is required.

    std::cout << argv[1] << std::endl;
    std::string filepath(argv[1]);

    CSVreader * reader = new CSVreader(filepath);

    std::string * resValues;
    while ((resValues = reader->parseLine()) != 0) {
        std::cout << "name: " << resValues[0] << std::endl;
        std::cout << "code: " << resValues[1] << std::endl;
        std::cout << "cost: " << resValues[2] << std::endl;
        std::cout << "-----------------" << std::endl;
    }

    delete reader;
    return 0;
}
