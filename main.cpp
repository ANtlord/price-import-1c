#include <iostream>
#include "include/csvreader.h"

int main(int argc, char *argv[])
{
    if (argc > 1) {
        std::cout << argv[1] << std::endl;
    }
    std::string filepath(argv[1]);

    CSVreader * reader = new CSVreader(filepath);
    

    uint32_t i = 0;
    while (reader->readline(filepath)) {
        std::cout << filepath << std::endl;
    }
    delete reader;
    return 0;
}
