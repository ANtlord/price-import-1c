#include <cxxtest/TestSuite.h>
#include "../include/FieldCoordinates.h"

using std::make_pair;

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

private:
    /* data */
};
