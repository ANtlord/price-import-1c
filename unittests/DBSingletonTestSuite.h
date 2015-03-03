#include <cxxtest/TestSuite.h>
#include "../include/DBSingleton.h"
#include "../include/ConfigSingleton.h"
#include <stdexcept>
#include <pqxx/pqxx>
#include <string.h>
#include <join.h>

using pqxx::work;
using std::string;

class DBSingletonTestSuite : public CxxTest::TestSuite
{
    const string _INDUSTRY_TABLE_NAME = "service_industry";
    const string _CATEGORY_TABLE_NAME = "service_category";
    const string _PRODUCT_TABLE_NAME = "service_product";
    const string _COMPANY_TABLE_NAME = "service_company";

public:
    DBSingletonTestSuite()
    {
        ConfigSingleton::getSingleton()
            ->addOption(Options::COMPANY_ID, "1")
            ->addOption(Options::DB_NAME, "test_base_price_import")
            ->addOption(Options::DB_USER, "postgres")
            ->addOption(Options::PRODUCT_TABLE, _PRODUCT_TABLE_NAME)
            ->addOption(Options::CATEGORY_TABLE, _CATEGORY_TABLE_NAME);
    }

    ~DBSingletonTestSuite()
    {
        work w(*(DBSingleton::getSingleton()->getConnection()));
        w.exec("delete from "+_INDUSTRY_TABLE_NAME);
        w.exec("delete from "+_COMPANY_TABLE_NAME);
        w.exec("delete from "+_CATEGORY_TABLE_NAME);
        w.exec("delete from "+_PRODUCT_TABLE_NAME);
        w.commit();
    }

    void setUp()
    {

    }

    string insertEntry(const string& table, const string fields[],
            const string values[], const size_t N)
    {
        auto dbs = DBSingleton::getSingleton();
        work w(*dbs->getConnection());
        w.exec("insert into "+table+" ("+forge::join(fields, N, ", ")+")"
                +" values ("+forge::join(values, N, ", ")+")");
        w.commit();

        work getw(*dbs->getConnection());
        auto res = getw.exec("select id from "+table+" where "
                +fields[0]+"="+values[0]+";");
        return res[0][0].as<string>();
    }

    void testGetTableData()
    {
        size_t n = 3;
        string fields[n] = {"name", "sort", "is_active"};
        string values[n] = {"'Telecomminication'", "'300'", "'true'"};
        auto id = insertEntry(_INDUSTRY_TABLE_NAME, fields, values, n);

        n=5;
        string cfields[n] = {"name", "sort", "is_active", "section_id", "created_at"};
        string cvalues[n] = {"'SuperIT'", "'300'", "'true'",
            "'"+id+"'", "'2015-02-28'"};
        id = insertEntry(_COMPANY_TABLE_NAME, cfields, cvalues, n);
        ConfigSingleton::getSingleton()->addOption(Options::COMPANY_ID, id);

        n=4;
        string sfields[n] = {"name", "sort", "is_active", "section_id"};
        string svalues[n] = {"'High load job'", "'300'", "'true'", "'"+id+"'"};
        id = insertEntry(_CATEGORY_TABLE_NAME, sfields, svalues, n);

        auto res = DBSingleton::getSingleton()->getTableData(_CATEGORY_TABLE_NAME,
                new string[1]{"name"}, 1);
        TS_ASSERT_EQUALS(res.size(), 1);

        for (const auto& item: res) {
            uint32_t i = 0;
            for (const auto& subItem: item) {
                TS_ASSERT_EQUALS("'"+subItem+"'", svalues[i]);
                ++i;
            } 
        }
    }
};
