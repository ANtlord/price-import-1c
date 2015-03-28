#include <cxxtest/TestSuite.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include "../include/csvreader.h"
#include <array>

using std::string;
using std::fstream;
using std::array;

class CSVreaderTestSuite : public CxxTest::TestSuite
{
private:
    string _TEST_FILE_PATH;
    array<string, 8> _testAggregateData = {{
        "Dolls;\n", "Toy Superman;\n", "130902;27,00 \n", "Toy Batman;\n", "100901;30,02 \n",
        "Auto models;\n", "Toy BMW i8;\n", "791000; 15,00 \n"
    }};

    ReaderOptions * _optionsPlaceholder;
    string _curDir;

public:
    CSVreaderTestSuite()
    {
        _optionsPlaceholder = new ReaderOptions(0,0,0,0,0);
        char *buf = new char[256];
        getcwd(buf, 256);
        _curDir = string(buf);

        _TEST_FILE_PATH = _curDir+"/unittest_csvfile.csv";

        std::ofstream file(_TEST_FILE_PATH);
        if (file.good()) {
            for (const auto& item: _testAggregateData) {
                file << item;
            }
            file.close();
        }
        else {
            TS_FAIL("Cannot open file for writing "+_TEST_FILE_PATH);
        }
    }

    void testReadline()
    {
        string buffer;
        auto reader = new CSVreader(_TEST_FILE_PATH, *_optionsPlaceholder);
        TS_ASSERT_EQUALS(reader->readline(buffer), true);
        TS_ASSERT_EQUALS((buffer+"\n"), _testAggregateData.at(0));
        delete reader;

        reader = new CSVreader(_TEST_FILE_PATH, *_optionsPlaceholder);
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
                /*numCol*/ N, /*entryLines*/ 0, /*isCascad*/ 0);
        auto reader = new CSVreader(_TEST_FILE_PATH, options);
        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0]+";\n", _testAggregateData[1]);
        TS_ASSERT_EQUALS(buffer[1], "130902");
        TS_ASSERT_EQUALS(buffer[2], "27.00");
        TS_ASSERT_EQUALS(buffer[3]+";\n", _testAggregateData[0]);

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0]+";\n", _testAggregateData[3]);
        TS_ASSERT_EQUALS(buffer[1], "100901");
        TS_ASSERT_EQUALS(buffer[2], "30.02");
        TS_ASSERT_EQUALS(buffer[3]+";\n", _testAggregateData[0]);
        delete reader;
        delete[] buffer;
    }

    void testParselineFromNonFirstLine()
    {
        // Tests correct number of start line.
        const uint32_t N = 4;
        auto options = ReaderOptions(/*startLine*/ 4, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 0, /*isCascad*/ 0);
        auto reader = new CSVreader(_TEST_FILE_PATH, options);
        string * buffer = new string[N];
        buffer = reader->parseLine();
        
        TS_ASSERT_EQUALS(buffer[0]+";\n", _testAggregateData[6]);
        TS_ASSERT_EQUALS(buffer[1], "791000");
        TS_ASSERT_EQUALS(buffer[2], "15.00");
        TS_ASSERT_EQUALS(buffer[3]+";\n", _testAggregateData[5]);
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
        const uint32_t N = 4;
        ReaderOptions options(/*startLine*/ 0, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 0, /*isCascad*/ 0);
        auto reader = new CSVreader(FILEPATH, options);

        string * buffer = new string[N];

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0], "Toy Batman");
        TS_ASSERT_EQUALS(buffer[1], "130902");
        TS_ASSERT_EQUALS(buffer[2], "26.00");
        TS_ASSERT_EQUALS(buffer[3], "Toys");

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0], "Toy Superman");
        TS_ASSERT_EQUALS(buffer[1], "421013");
        TS_ASSERT_EQUALS(buffer[2], "204.00");
        TS_ASSERT_EQUALS(buffer[3], "Toys");

        remove(FILEPATH.c_str());
        delete reader;
        delete[] buffer;
    }

    virtual ~CSVreaderTestSuite()
    {
        remove(_TEST_FILE_PATH.c_str());
    }
};
