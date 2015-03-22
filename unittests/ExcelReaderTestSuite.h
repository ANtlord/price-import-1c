#include <cxxtest/TestSuite.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include "../include/ExcelReader.h"
#include <array>

using std::string;
using std::fstream;
using std::array;

class ExcelReaderTestSuite : public CxxTest::TestSuite
{
private:
    string _TEST_FILE_PATH;

public:
    ExcelReaderTestSuite()
    {
        char *buf = new char[256];
        getcwd(buf, 256);
        _TEST_FILE_PATH = string(buf)+"/unittests/price_simple.xls";
    }

    void testReadline()
    {
        string buffer;
        auto reader = new ExcelReader(_TEST_FILE_PATH);
        TS_ASSERT_EQUALS(reader->readline(buffer), false);
    }
    
    void testParseline()
    {
        string * buffer = new string[4];
        auto reader = new ExcelReader(_TEST_FILE_PATH);
        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0], "Toy SuperMan. Made in China 2012");
        TS_ASSERT_EQUALS(buffer[1], "130902.000000");
        TS_ASSERT_EQUALS(buffer[2], "26.000000");
        TS_ASSERT_EQUALS(buffer[3], "Dolls");

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0], "Toy Batman. Weight 116g. Made in Canada 2013");
        TS_ASSERT_EQUALS(buffer[1], "421013.000000");
        TS_ASSERT_EQUALS(buffer[2], "204.000000");
        TS_ASSERT_EQUALS(buffer[3], "Dolls");
    }

    virtual ~ExcelReaderTestSuite()
    {
    }
};
