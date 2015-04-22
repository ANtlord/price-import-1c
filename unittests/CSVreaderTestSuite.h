#include <cxxtest/TestSuite.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include "../include/csvreader.h"
#include <array>
#include <split.h>
#include <trim.h>

using std::string;
using std::fstream;
using std::array;
using std::make_pair;

class CSVreaderTestSuite : public CxxTest::TestSuite
{
private:
    string _TEST_FILE_PATH;
    array<string, 8> _testAggregateData = {{
        "Dolls;\n",
        "Toy Superman;\n", "130902;27,00 \n",
        "Toy Batman;\n", "100901;30,02 \n",
        "Auto models;\n",
        "Toy BMW i8;\n", "791000; 15,00 \n"
    }};
    FieldCoordinates * _coords = nullptr;

    ReaderOptions * _optionsPlaceholder = nullptr;
    string _curDir;

    template<size_t N>
    void _fillFile(const string &path, const array<string, N>& arr)
    {
        std::ofstream file(path);
        if (file.good()) {
            for (const auto& item: arr) {
                file << item;
            }
            file.close();
        }
        else {
            TS_FAIL("Cannot open file for writing "+path);
        }
    }

public:
    CSVreaderTestSuite()
    {
        const uint32_t N = 4;
        _coords = new FieldCoordinates(
            make_pair(0,0), /* section coordinates */
            make_pair(0,1), /* name coordinates */
            make_pair(0,2), /* code coordinates */
            make_pair(1,2)  /* price coordinates */
        );
        _optionsPlaceholder = new ReaderOptions(/*startLine*/ 0, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 3, /*isCascad*/ true);
        char *buf = new char[256];
        getcwd(buf, 256);
        _curDir = string(buf);

        _TEST_FILE_PATH = _curDir+"/unittest_csvfile.csv";

        _fillFile(_TEST_FILE_PATH, _testAggregateData);
    }

    void testReadline()
    {
        string buffer;

        auto reader = new CSVreader(_TEST_FILE_PATH, *_optionsPlaceholder, *_coords);
        TS_ASSERT_EQUALS(reader->readline(buffer), true);
        TS_ASSERT_EQUALS((buffer+"\n"), _testAggregateData.at(0));
        delete reader;

        reader = new CSVreader(_TEST_FILE_PATH, *_optionsPlaceholder, *_coords);
        uint32_t i = 0;
        while (reader->readline(buffer)) ++i;
        TS_ASSERT_EQUALS(i, _testAggregateData.size());
        TS_ASSERT_EQUALS(reader->readline(buffer), true);
        delete reader;
    }
    
    void testParselineFromFirstLine()
    {
        const uint32_t N = 4;
        string * buffer = new string[N];
        auto options = ReaderOptions(/*startLine*/ 0, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 3, /*isCascad*/ true);
        auto reader = new CSVreader(_TEST_FILE_PATH, options, *_coords);
        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME]+";\n", _testAggregateData[1]);
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "130902");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "27.00");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION]+";\n", _testAggregateData[0]);

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME]+";\n", _testAggregateData[3]);
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "100901");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "30.02");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION]+";\n", _testAggregateData[0]);

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME]+";\n", _testAggregateData[6]);
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "791000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "15.00");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION]+";\n", _testAggregateData[5]);

        TS_ASSERT(nullptr == reader->parseLine());
        delete reader;
        delete[] buffer;
    }

    void testParselineFromNonFirstLine()
    {
        // Tests correct number of start line.
        const uint32_t N = 4;
        auto options = ReaderOptions(/*startLine*/ 4, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 3, /*isCascad*/ true);
        auto reader = new CSVreader(_TEST_FILE_PATH, options, *_coords);
        string * buffer = new string[N];
        buffer = reader->parseLine();
        
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME]+";\n", _testAggregateData[6]);
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "791000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "15.00");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION]+";\n", _testAggregateData[5]);
        delete reader;
        delete[] buffer;
    }
    
    void testParseDataWithCleanFields()
    {
        const string DATA = ";    Toys;;\n ;        Toy Batman;;\n ;130902;26,00 ;\n ;        Toy Superman;;\n ;421013;204,00 ;";
        const string FILEPATH = _curDir+"/clean_col_csv_file.csv";
        std::ofstream file(FILEPATH);
        if (file.good()) file << DATA;
        file.close();
        auto reader = new CSVreader(FILEPATH, *_optionsPlaceholder, *_coords);

        const uint32_t N = 4;
        string * buffer = new string[N];

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME], "Toy Batman");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "130902");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "26.00");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION], "Toys");

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME], "Toy Superman");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "421013");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "204.00");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION], "Toys");

        remove(FILEPATH.c_str());
        delete reader;
        delete[] buffer;
    }

    void testPlainParse()
    {
        // Preparing data.
        const array<string, 2> DATA = {{
            "Toys; Toy Superman; 130902; 26,00;\n",
            "Toys; Toy Batman; 110905; 138,00;\n"
        }};
        const string TEST_FILE_PATH = _curDir+"/unittest_plain_csvfile.csv";
        _fillFile(TEST_FILE_PATH, DATA);
        const uint32_t N = 4;
        FieldCoordinates coords(
            make_pair(0,0),
            make_pair(1,0),
            make_pair(2,0),
            make_pair(3,0)
        );
        ReaderOptions options(/*startLine*/ 0, /*startCol*/ 0, /*numCol*/ N,
                /*entryLines*/ 1, /*isCascad*/ false);
        auto reader = new CSVreader(TEST_FILE_PATH, options, coords);
        std::string * resValues;

        uint32_t i = 0;
        while ((resValues = reader->parseLine()) != nullptr){
            auto values = forge::split(DATA.at(i), ';');
            for (uint8_t j = 0; j < values.size(); ++j) {
                forge::trim(values[j]);
                if (j == coords.getPrice().first)
                    std::replace(values[j].begin(), values[j].end(), ',', '.');
            }

            TS_ASSERT_EQUALS(resValues[ResultIndexes::NAME],
                    values.at(coords.getName().first));
            TS_ASSERT_EQUALS(resValues[ResultIndexes::CODE],
                    values.at(coords.getCode().first));
            TS_ASSERT_EQUALS(resValues[ResultIndexes::PRICE],
                    values.at(coords.getPrice().first));
            TS_ASSERT_EQUALS(resValues[ResultIndexes::SECTION],
                    values.at(coords.getCategory().first));
            delete[] resValues;
            ++i;
        }
        
        delete reader;
        remove(TEST_FILE_PATH.c_str());
    }

    virtual ~CSVreaderTestSuite()
    {
        remove(_TEST_FILE_PATH.c_str());
    }
};
