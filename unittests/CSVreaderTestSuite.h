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
    array<string, 5> _testAggregateData = {{"Dolls;\n", "Toy Superman;\n",
        "130902;27,00 \n", "Toy Batman;\n", "100901;30,02 \n"}};

public:
    CSVreaderTestSuite()
    {
        char *buf = new char[256];
        getcwd(buf, 256);
        _TEST_FILE_PATH = string(buf)+"/unittest_csvfile.csv";

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
        auto reader = new CSVreader(_TEST_FILE_PATH);
        TS_ASSERT_EQUALS(reader->readline(buffer), true);
        TS_ASSERT_EQUALS((buffer+"\n"), _testAggregateData.at(0));
        delete reader;

        reader = new CSVreader(_TEST_FILE_PATH);
        uint32_t i = 0;
        while (reader->readline(buffer)) ++i;
        TS_ASSERT_EQUALS(i, _testAggregateData.size());
        TS_ASSERT_EQUALS(reader->readline(buffer), true);
        delete reader;
    }
    
    void testParseline()
    {
        string * buffer = new string[4];
        auto reader = new CSVreader(_TEST_FILE_PATH);
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
    }

    virtual ~CSVreaderTestSuite()
    {
        remove(_TEST_FILE_PATH.c_str());
    }
};
