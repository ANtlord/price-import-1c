#include <cxxtest/TestSuite.h>
#include "../include/ConfigSingleton.h"
#include <stdexcept>

class ConfigSingletonTestSuite : public CxxTest::TestSuite
{
public:
    void setUp()
    {
    }

    void testAdditionValues()
    {
        auto conf = ConfigSingleton::getSingleton();
        conf->addOption(Options::COMPANY_ID, "1");
        TS_ASSERT_EQUALS(conf->getOption(Options::COMPANY_ID), "1");
    }
};
