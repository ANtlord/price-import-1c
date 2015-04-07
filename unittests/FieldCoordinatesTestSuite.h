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

private:
    /* data */
};
