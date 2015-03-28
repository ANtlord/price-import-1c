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
        ReaderOptions options(/*startLine*/ 0, /*startCol*/ 0,
                /*numCol*/ 4, /*entryLines*/ 0, /*isCascad*/ 0);
        auto reader = new ExcelReader(_TEST_FILE_PATH, options);
        TS_ASSERT_EQUALS(reader->readline(buffer), false);
    }

    void testParselineFromFirstLine()
    {
        const uint32_t N = 4;
        ReaderOptions options(/*startLine*/ 0, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 0, /*isCascad*/ 0);
        string * buffer = new string[N];
        auto reader = new ExcelReader(_TEST_FILE_PATH, options);
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
    
    void testParselineFromNonFirstLine()
    {
        const uint32_t N = 4;
        ReaderOptions options(/*startLine*/ 5, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 0, /*isCascad*/ 0);
        string * buffer = new string[N];
        auto reader = new ExcelReader(_TEST_FILE_PATH, options);
        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0], "Toy BMW i8");
        TS_ASSERT_EQUALS(buffer[1], "777109.000000");
        TS_ASSERT_EQUALS(buffer[2], "10.500000");
        TS_ASSERT_EQUALS(buffer[3], "Auto");

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[0], "Toy Subaru Forester");
        TS_ASSERT_EQUALS(buffer[1], "618022.000000");
        TS_ASSERT_EQUALS(buffer[2], "80.500000");
        TS_ASSERT_EQUALS(buffer[3], "Auto");
        delete reader;
    }


    virtual ~ExcelReaderTestSuite()
    {
    }
};
