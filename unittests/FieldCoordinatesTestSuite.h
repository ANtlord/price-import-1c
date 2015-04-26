#include <cxxtest/TestSuite.h>
#include "../include/FieldCoordinates.h"

using std::make_pair;
typedef std::pair<uint8_t, uint8_t> cpair;

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
            new cpair(0,0), /* section coordinates */
            new cpair(0,1), /* name coordinates */
            new cpair(0,2), /* code coordinates */
            new cpair(1,2)  /* price coordinates */
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

    void testGetFieldsAsArray()
    {
        cpair arr[] = {
            make_pair(0,1), /* name coordinates */
            make_pair(0,2), /* code coordinates */
            make_pair(1,2), /* price coordinates */
            make_pair(0,0) /* section coordinates */
        };

        FieldCoordinates coords(
            &arr[3], &arr[0], &arr[2], &arr[1]
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
