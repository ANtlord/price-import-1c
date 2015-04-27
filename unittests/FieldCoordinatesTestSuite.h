#include <cxxtest/TestSuite.h>
#include "../include/FieldCoordinates.h"
#include <array>
#include <fstream>

using std::make_pair;
using std::array;
using std::string;
using std::pair;
using std::make_pair;

typedef pair<uint8_t, uint8_t> cpair;

class FieldCoordinatesTestSuite : public CxxTest::TestSuite
{
public:
    FieldCoordinatesTestSuite()
    {
    }

    virtual ~FieldCoordinatesTestSuite(){

    }

    void testGetResultIndex()
    {
        FieldCoordinates coords(
            make_pair(0,0), /* section coordinates */
            make_pair(0,1), /* name coordinates */
            make_pair(0,2), /* code coordinates */
            make_pair(1,2)  /* price coordinates */
        );

        try {
            coords.getResultIndex(1,1);
            TS_ASSERT(false);
        }
        catch (ResultIndexesException &e) {
            e.what(1,1);
            TS_ASSERT(true);
        }
    }

    void testgetFieldsAsArray()
    {
        std::pair<uint8_t, uint8_t> arr[] = {
            make_pair(0,1), /* name coordinates */
            make_pair(0,2), /* code coordinates */
            make_pair(1,2), /* price coordinates */
            make_pair(0,0) /* section coordinates */
        };

        FieldCoordinates coords(
            arr[3], arr[0], arr[1], arr[2]
        );

        auto arr2 = coords.getFieldsAsArray();
        for (int i = 0; i < coords.getFieldsNum(); ++i) {
            TS_ASSERT_EQUALS(arr2[i]->first, arr[i].first);
            TS_ASSERT_EQUALS(arr2[i]->second, arr[i].second);
        }
    }

    void testFileConstructor()
    {
        const array<pair<string, cpair>, 4> DATA = {{
            make_pair("name", cpair(0,1)),
            make_pair("code", cpair(0,2)),
            make_pair("price", cpair(1,2)),
            make_pair("category", cpair(0,0))
        }};

        string filepath(realpath("/proc/self/exe", NULL));
        filepath = filepath.substr(0, filepath.rfind('/'));
        filepath += "/unittests/coordinates.txt";

        std::ofstream file(filepath);
        if (file.good()) {
            for (const auto& item: DATA)
                file << item.first << " = " << (int)item.second.first << ","
                    << (int)item.second.second << std::endl;
            file.close();
        }
        else TS_FAIL("Cannot open file for writing "+filepath);

        FieldCoordinates coords(filepath);
        auto arr = coords.getFieldsAsArray();
        for (int i = 0; i < coords.getFieldsNum(); ++i) {
            TS_ASSERT_EQUALS(DATA[i].second.first, arr[i]->first);
            TS_ASSERT_EQUALS(DATA[i].second.second, arr[i]->second);
        }
    }
private:
    /* data */
};
