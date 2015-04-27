#include <iostream>
#include "include/csvreader.h"
#include "include/ExcelReader.h"
#include <cassert>
#include "include/SaveCommand.h"
#include "include/DBSingleton.h"
#include "include/ConfigSingleton.h"
#include <easylogging++.h>
#include <stdlib.h>
#include <unistd.h>
#include <tclap/CmdLine.h>

using std::make_pair;
using std::string;
INITIALIZE_EASYLOGGINGPP
int log_sequence = 1;

void LogsRollout(const char* fname, size_t fsize)
{	
    string fileName = fname;
    size_t position = fileName.find(".");
    string extractName = (string::npos == position)? fileName : fileName.substr(0, position);
    extractName = extractName + ".log." + std::to_string(log_sequence++);
    
    int status = rename(fname, extractName.c_str());	
    if(status)
    {
        // Something wrong.
    }
}

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmd("Programm for importing data.", ' ', "0.1");
    TCLAP::ValueArg<string> formatArg("f","format","format of file",
            /* is requirement*/true, "csv", "string");
    TCLAP::ValueArg<uint64_t> companyidArg("o","organization","id of company",
            /* is requirement*/true, 0, "int");
    TCLAP::ValueArg<string> pathArg("p","path","path to file for import",
            /* is requirement*/true, "import.csv", "string");
    TCLAP::ValueArg<uint64_t> startLineArg("s","start","number of start line",
            /* is requirement*/true, 0, "int");
    TCLAP::ValueArg<string> dbArg("d","database","name of database",
            /* is requirement*/true, "maindb_test", "string");
    TCLAP::ValueArg<string> dbuserArg("u","dbuser","username of database",
            /* is requirement*/true, "postgres", "string");
    TCLAP::ValueArg<string> productTableArg("g","product_table","name of product table",
            /* is requirement*/true, "service_product", "string");
    TCLAP::ValueArg<string> categoryTableArg("c","category_table","name of category table",
            /* is requirement*/true, "service_category", "string");
    TCLAP::ValueArg<bool> isCascadArg("i","cascad","point about type of import data",
            /* is requirement*/true, true, "bool");
    TCLAP::ValueArg<uint16_t> entryLinesArg("n","lines",
            "number of lines of information about product",
            /* is requirement*/true, 0, "int");
    TCLAP::ValueArg<string> coordsArg("r","coords","path to file with coordinates",
            /* is requirement*/true, "coords.txt", "string");

    cmd.add( formatArg );
    cmd.add( companyidArg );
    cmd.add( pathArg );
    cmd.add( startLineArg );
    cmd.add( dbArg );
    cmd.add( dbuserArg );
    cmd.add( productTableArg );
    cmd.add( categoryTableArg );
    cmd.add( isCascadArg );
    cmd.add( entryLinesArg );
    cmd.add( coordsArg );
    cmd.parse( argc, argv );

    assert(isCascadArg.getValue());
    assert(entryLinesArg.getValue() < UINT8_MAX);

    auto conf = ConfigSingleton::getSingleton();
    // TODO: check company in database.
    const string FILEPATH(conf->getOption(Options::PATH)+"/"+pathArg.getValue());
    assert(companyidArg.getValue() > 0);

    std::ifstream file(FILEPATH);
    assert(file);
    file.close();

    const string COMPANY_ID = std::to_string(companyidArg.getValue());
    const string CATEGORY_TABLE_NAME = categoryTableArg.getValue();
    const string PRODUCT_TABLE_NAME = productTableArg.getValue();
    const string EXTENSION(formatArg.getValue());

    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::MaxLogFileSize,
            "1048576" /* 1mb */);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Filename,
            conf->getOption(Options::PATH)+"/logs/"+COMPANY_ID+"/"+COMPANY_ID+".log");
    el::Helpers::installPreRollOutCallback(LogsRollout);
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);


    // Saving options of the programm.
    conf
        ->addOption(Options::COMPANY_ID, COMPANY_ID)
        ->addOption(Options::DB_NAME, dbArg.getValue())
        ->addOption(Options::DB_USER, dbuserArg.getValue())
        ->addOption(Options::PRODUCT_TABLE, PRODUCT_TABLE_NAME)
        ->addOption(Options::CATEGORY_TABLE, CATEGORY_TABLE_NAME);

    LOG(INFO) << "file: " << conf->getOption(Options::PATH)+"/"+pathArg.getValue();
    LOG(INFO) << "company ID: " << companyidArg.getValue();
    LOG(INFO) << "start line: " << startLineArg.getValue();

    DataFileReader * reader = nullptr;
    const size_t N = 4;
    auto obj = ReaderOptions(/*startLine*/ startLineArg.getValue(),
            /*startCol*/ 0, /*numCol*/ N,
            /*entryLines*/ entryLinesArg.getValue(),
            /*isCascad*/ isCascadArg.getValue());
    FieldCoordinates coords(conf->getOption(Options::PATH)+"/"
            +coordsArg.getValue());

    if (EXTENSION == "csv")
        reader = new CSVreader(FILEPATH, obj, coords);
    else if (EXTENSION == "xls" || EXTENSION == "xlsx")
        reader = new ExcelReader(FILEPATH, obj, coords);
    assert(reader != nullptr);  // Extension is not supporting.

    // Initializing reader of file.
    std::string * resValues;

    // Setting up parameters for parsing and storing categories.
    std::string categoryName = "";
    std::string * fields = new std::string[N]{
        "name", "section_id", "sort", "is_active"
    };
    SaveCommand * categorySaveCmd = new SaveCommand(CATEGORY_TABLE_NAME.c_str(),
            fields, N, "name");

    // Parsing and saving categories.
    while ((resValues = reader->parseLine()) != nullptr) {
        if (categoryName != resValues[ResultIndexes::SECTION]) {
            categoryName = resValues[ResultIndexes::SECTION];
            std::string * data = new std::string[N]{
                resValues[ResultIndexes::SECTION], COMPANY_ID, "300", "true"
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
    while ((resValues = reader->parseLine()) != nullptr) {
        if (categoryName != resValues[ResultIndexes::SECTION]) {
            categoryName = resValues[ResultIndexes::SECTION];

            // Gets id of category.
            for (auto it = CATEGORIES.begin(); it != CATEGORIES.end(); ++it) {
                if (it->at(1) == categoryName) {
                    categoryId = it->at(0);
                }
            }
        }
        assert(categoryId != "");
        std::string * data = new std::string[PRODUCT_N]{
            resValues[ResultIndexes::NAME], resValues[ResultIndexes::CODE],
            resValues[ResultIndexes::PRICE], categoryId, "300", "true"
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
