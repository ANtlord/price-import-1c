#include <cxxtest/TestSuite.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include "../include/ExcelReader.h"
#include <array>

using std::string;
using std::fstream;
using std::array;
using std::make_pair;

class ExcelReaderTestSuite : public CxxTest::TestSuite
{
private:
    string _TEST_FILE_PATH;
    FieldCoordinates * _coords = nullptr;

public:
    ExcelReaderTestSuite()
    {
        _coords = new FieldCoordinates(
            make_pair(0,0), /* section coordinates */
            make_pair(0,1), /* name coordinates */
            make_pair(0,2), /* code coordinates */
            make_pair(1,2)  /* price coordinates */
        );
        char *buf = new char[256];
        getcwd(buf, 256);
        _TEST_FILE_PATH = string(buf)+"/unittests/price_simple.xls";
    }

    void testReadline()
    {
        string buffer;
        ReaderOptions options(/*startLine*/ 0, /*startCol*/ 0,
                /*numCol*/ 4, /*entryLines*/ 3, /*isCascad*/ 0);
        auto reader = new ExcelReader(_TEST_FILE_PATH, options, *_coords);
        TS_ASSERT_EQUALS(reader->readline(buffer), false);
    }

    void testParselineFromFirstLine()
    {
        const uint32_t N = 4;
        ReaderOptions options(/*startLine*/ 0, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 3, /*isCascad*/ 0);
        string * buffer = new string[N];
        auto reader = new ExcelReader(_TEST_FILE_PATH, options, *_coords);
        buffer = reader->parseLine();
        
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME], "Toy SuperMan. Made in China 2012");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "130902.000000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "26.000000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION], "Dolls");

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME], "Toy Batman. Weight 116g. Made in Canada 2013");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "421013.000000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "204.000000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION], "Dolls");
    }
    
    void testParselineFromNonFirstLine()
    {
        const uint32_t N = 4;
        ReaderOptions options(/*startLine*/ 5, /*startCol*/ 0,
                /*numCol*/ N, /*entryLines*/ 3, /*isCascad*/ 0);
        string * buffer = new string[N];
        auto reader = new ExcelReader(_TEST_FILE_PATH, options, *_coords);
        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME], "Toy BMW i8");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "777109.000000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "10.500000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION], "Auto");

        buffer = reader->parseLine();
        TS_ASSERT_EQUALS(buffer[ResultIndexes::NAME], "Toy Subaru Forester");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::CODE], "618022.000000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::PRICE], "80.500000");
        TS_ASSERT_EQUALS(buffer[ResultIndexes::SECTION], "Auto");
        delete reader;
    }


    virtual ~ExcelReaderTestSuite()
    {
    }
};
