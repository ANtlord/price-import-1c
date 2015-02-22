#include <iostream>
#include "include/csvreader.h"
#include "include/ExcelReader.h"
#include <cassert>
#include "include/SaveCommand.h"
#include "include/DBSingleton.h"
#include "include/ConfigSingleton.h"

int main(int argc, char *argv[])
{
    const std::string CATEGORY_TABLE_NAME = "service_category";
    const std::string PRODUCT_TABLE_NAME = "service_product";
    const std::string COMPANY_TABLE_NAME = "service_company";

    assert(argc > 3);   // Specifying filepath is required.

    // Saving options of the programm.
    ConfigSingleton::getSingleton()
        ->addOption("company_id", std::string(argv[2]))
        ->addOption("db_name", "maindb_test")
        ->addOption("db_user", "postgres")
        ->addOption("product_table", PRODUCT_TABLE_NAME)
        ->addOption("category_table", CATEGORY_TABLE_NAME);

    std::cout << "file: " << argv[1] << std::endl;
    std::cout << "company ID: " << argv[2] << std::endl;
    std::string filepath(argv[1]);

    const std::string EXTENSION(argv[3]);
    DataFileReader * reader = nullptr;
    if (EXTENSION == "csv")
        reader = new CSVreader(filepath);
    else if (EXTENSION == "xls" || EXTENSION == "xlsx")
        reader = new ExcelReader(filepath);
    assert(reader != nullptr);  // Extension is not supporting.
    return 0;

    // Initializing reader of file.
    std::string * resValues;

    // Setting up parameters for parsing and storing categories.
    std::string categoryName = "";
    const size_t N = 4;
    std::string * fields = new std::string[N]{
        "name", "section_id", "sort", "is_active"
    };
    SaveCommand * categorySaveCmd = new SaveCommand(CATEGORY_TABLE_NAME.c_str(),
            fields, N, "name");

    // Parsing and saving categories.
    while ((resValues = reader->parseLine()) != 0) {
        if (categoryName != resValues[3]) {
            categoryName = resValues[3];
            std::string * data = new std::string[N]{
                resValues[3], std::string(argv[2]), "300", "true"
            };
            categorySaveCmd->addData(data);
        }

        delete[] resValues;
    }
    categorySaveCmd->execute();
    
    // Setting up parameters for parsing and storing products.
    const size_t PRODUCT_N = 6;
    fields = new std::string[PRODUCT_N]{
        "name", "code", "price", "section_id", "sort", "is_active"
    };
    SaveCommand * productSaveCmd = new SaveCommand(PRODUCT_TABLE_NAME.c_str(),
            fields, PRODUCT_N, "code");

    // Getting ID numbers of the categories for association to numbers in
    // products data.
    std::string categoryFields[2] = {"id", "name"};
    auto db = DBSingleton::getSingleton();
    std::list<std::vector<std::string>> CATEGORIES = db->getTableData(
            CATEGORY_TABLE_NAME, categoryFields, 2);

    // Parsing and saving products.
    std::string categoryId = "";
    categoryName = "";
    while ((resValues = reader->parseLine()) != 0) {
        if (categoryName != resValues[3]) {
            categoryName = resValues[3];

            // Gets id of category.
            for (auto it = CATEGORIES.begin(); it != CATEGORIES.end(); ++it) {
                if (it->at(1) == categoryName) {
                    categoryId = it->at(0);
                }
            }
        }
        assert(categoryId != "");
        std::string * data = new std::string[PRODUCT_N]{
            resValues[0], resValues[1], resValues[2], categoryId, "300", "true"
        };
        productSaveCmd->addData(data);
        delete[] resValues;
    }
    bool res = productSaveCmd->execute();

    delete categorySaveCmd;
    delete productSaveCmd;
    delete reader;
    return 0;
}
