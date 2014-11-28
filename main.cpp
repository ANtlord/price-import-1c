#include <iostream>
#include "include/csvreader.h"
#include <cassert>
#include "include/SaveCommand.h"

int main(int argc, char *argv[])
{
    assert(argc > 1);   // Specifying filepath is required.

    std::cout << argv[1] << std::endl;
    std::string filepath(argv[1]);

    CSVreader * reader = new CSVreader(filepath);
    std::string * resValues;
    std::string categoryName = "";

    std::string * fields = new std::string[4];
    fields[0] = "name";
    fields[1] = "code";
    fields[2] = "price";
    fields[3] = "category_id";
    SaveCommand * productSaveCmd = new SaveCommand("product", fields, 4);

    fields = new std::string[1];
    fields[0] = "name";
    SaveCommand * categorySaveCmd = new SaveCommand("category", fields, 1);

    while ((resValues = reader->parseLine()) != 0) {
        if (categoryName != resValues[3]) {
            categoryName = resValues[3];
            std::string * data = new std::string[1];
            data[0] = resValues[3];
            categorySaveCmd->addData(data);
        }

        //productSaveCmd->addData(resValues);
        //std::cout << "name: " << resValues[0] << std::endl;
        //std::cout << "code: " << resValues[1] << std::endl;
        //std::cout << "cost: " << resValues[2] << std::endl;
        //std::cout << "section: " << resValues[3] << std::endl;
        //std::cout << "-----------------" << std::endl;
        delete[] resValues;
    }
    categorySaveCmd->execute();

    delete reader;
    return 0;
}
