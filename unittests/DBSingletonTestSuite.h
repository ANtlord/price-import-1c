#include <cxxtest/TestSuite.h>
#include "../include/DBSingleton.h"
#include "../include/ConfigSingleton.h"
#include <stdexcept>
#include <pqxx/pqxx>
#include <string.h>
#include <join.h>
#include <each.h>
#include <list>

using pqxx::work;
using std::string;
using std::list;

class DBSingletonTestSuite : public CxxTest::TestSuite
{
    const string _INDUSTRY_TABLE_NAME = "service_industry";
    const string _CATEGORY_TABLE_NAME = "service_category";
    const string _PRODUCT_TABLE_NAME = "service_product";
    const string _COMPANY_TABLE_NAME = "service_company";

    string _createIndustry()
    {
        const size_t N = 3;
        string fields[N] = {"name", "sort", "is_active"};
        string values[N] = {"'Telecomminication'", "'300'", "'true'"};
        return insertEntry(_INDUSTRY_TABLE_NAME, fields, values, N);
    }

    string _createCompany(string sectionId)
    {
        const size_t N = 5;
        string cfields[N] = {"name", "sort", "is_active", "section_id", "created_at"};
        string cvalues[N] = {"'SuperIT'", "'300'", "'true'",
            "'"+sectionId+"'", "'2015-02-28'"};
        return insertEntry(_COMPANY_TABLE_NAME, cfields, cvalues, N);
    }

    string * _createCategory(string sectionId)
    {
        const size_t N = 4;
        string sfields[N] = {"name", "sort", "is_active", "section_id"};
        string *svalues = new string[N]{
            "'High load job'", "'300'", "'true'", "'"+sectionId+"'"
        };
        insertEntry(_CATEGORY_TABLE_NAME, sfields, svalues, N);
        return svalues;
    }

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
    }

    void setUp()
    {

    }

    void tearDown()
    {
        work w(*(DBSingleton::getSingleton()->getConnection()));
        w.exec("delete from "+_INDUSTRY_TABLE_NAME);
        w.exec("delete from "+_COMPANY_TABLE_NAME);
        w.exec("delete from "+_CATEGORY_TABLE_NAME);
        w.exec("delete from "+_PRODUCT_TABLE_NAME);
        w.commit();
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
        auto id = _createIndustry();
        id = _createCompany(id);
        ConfigSingleton::getSingleton()->addOption(Options::COMPANY_ID, id);
        string * svalues = _createCategory(id);

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
        delete[] svalues;
    }
    
    void testCheckEntry()
    {
        auto id = _createIndustry();
        id = _createCompany(id);
        ConfigSingleton::getSingleton()->addOption(Options::COMPANY_ID, id);
        _createCategory(id);

        auto dbs = DBSingleton::getSingleton();
        work w(*dbs->getConnection());
        bool flag = DBSingleton::getSingleton()->checkEntry(
            _CATEGORY_TABLE_NAME, "name", "High load job", w
        );
        TS_ASSERT_EQUALS(flag, true);

        flag = DBSingleton::getSingleton()->checkEntry(_CATEGORY_TABLE_NAME,
                "name", "qweasd", w);
        w.commit();
        TS_ASSERT_EQUALS(flag, false);
    }

    /**
     * Tests updating of category and product.
     */
    void testUpdateEntry()
    {
        // Tests category.
        auto id = _createIndustry();
        id = _createCompany(id);
        ConfigSingleton::getSingleton()->addOption(Options::COMPANY_ID, id);
        string * svalues =  _createCategory(id);

        auto dbs = DBSingleton::getSingleton();
        work w(*dbs->getConnection());
        size_t N = 2;
        bool flag = dbs->updateEntry(_CATEGORY_TABLE_NAME,
                new string[N]{"name", "sort"}, new string[N]{"High load job",
                "299"}, N, "name", w);
        TS_ASSERT_EQUALS(flag, true);

        auto res = w.exec("select name, sort, id from "+_CATEGORY_TABLE_NAME
                +" where name = "+svalues[0] + " and section_id = "+id);
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "High load job");
        TS_ASSERT_EQUALS(res[0][1].as<string>(), "299");

        auto res2 = w.exec("select count(1) from "+_CATEGORY_TABLE_NAME
                +" where name = " +svalues[0] + " and section_id = "+id);
        TS_ASSERT_EQUALS(res2[0][0].as<string>(), "1");

        // Testing product.
        const size_t PRODUCT_N = 6;
        
        auto fields = new string[PRODUCT_N]{
            "name", "code", "price", "section_id", "sort", "is_active"
        };
        auto categoryId = res[0][2].as<string>();
        const string * values = new string[PRODUCT_N]{
            "Forwarding", "100122", "30.00", categoryId, "300", "true"
        };

        string * values2 = forge::each<string>([&w](const string& v)-> string {return w.quote(v);},
                values, (uint32_t)PRODUCT_N);

        res = w.exec("insert into " + _PRODUCT_TABLE_NAME + "("
                +forge::join(fields, PRODUCT_N, ", ")+")" + " values ("
                +forge::join(values2, PRODUCT_N, ", ")+")");

        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "code"},
                new string[N]{"qweasd", values[1]}, N, "code", w);

        res = w.exec("select name from "+_PRODUCT_TABLE_NAME +" where code = "
                +w.quote(values[1]) + " and section_id = "+categoryId);
        TS_ASSERT_EQUALS(flag, true);
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "qweasd");
        res = w.exec("select count(1) from "+_PRODUCT_TABLE_NAME +" where code = "
                +w.quote(values[1]) + " and section_id = "+categoryId);
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "1");
        w.commit();

        // Tests incorrect keyfield
        work w5(*dbs->getConnection());
        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "code"},
                new string[N]{"qweasd", values[1]}, N, "fake_field", w5);
        TS_ASSERT_EQUALS(flag, false);
        std::cout << "end of the line" << std::endl;
        w5.commit();

        // This is not correct test, because entry with code, which equals fake_value. 
        work w6(*dbs->getConnection());
        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "code"},
                new string[N]{"qweasd", "fake_value"}, N, "code", w6);
        TS_ASSERT_EQUALS(flag, true);
        w6.commit();

        // Tests incorrect fieldsNum.
        work w2(*dbs->getConnection());
        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "code"},
                new string[N]{"qweasd", values[1]}, 0, "fake_field", w2);
        TS_ASSERT_EQUALS(flag, false);

        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "code"},
                new string[N]{"qweasd", values[1]}, 1, "fake_field", w2);
        TS_ASSERT_EQUALS(flag, false);

        // checking database values.
        res = w2.exec("select count(1) from "+_PRODUCT_TABLE_NAME +" where code = "
                +w2.quote(values[1]) + " and section_id = "+categoryId);
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "1");

        // Tests throwing of transaction by incorrect key field. NOTICE: if
        // transaction will throw, than it will not be used. Therefore I create
        // another transaction.
        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "fake_field"},
                new string[N]{"qweasd", values[1]}, 2, "fake_field", w2);
        TS_ASSERT_EQUALS(flag, false);

        // Tests throwing of transaction by updating entry, which doesn't exist.
        work w3(*dbs->getConnection());
        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "fake_field"},
                new string[N]{"qweasd", values[1]}, 2, "name", w3);
        TS_ASSERT_EQUALS(flag, false);

        // Tests throwing of transaction by pointing index of key field,
        // which equals number of fields.
        work w4(*dbs->getConnection());
        flag = dbs->updateEntry(_PRODUCT_TABLE_NAME, new string[N]{"name", "fake_field"},
                new string[N]{"qweasd", values[1]}, 2, "name", w4, 2);
        TS_ASSERT_EQUALS(flag, false);

        delete[] fields;
        delete[] values;
        delete[] values2;
    }

    void testInsertEntryList()
    {
        auto dbs = DBSingleton::getSingleton();

        // Tests insertion, which contains correct values.
        work w(*dbs->getConnection());
        const uint32_t N = 3;
        string fields[N] = {"name", "sort", "is_active"};
        string * ind1Values = new string[N]{"Telecomminication", "300", "true"};
        string * ind2Values = new string[N]{"Fast Food", "200", "true"};
        std::list<string*> values;
        values.push_back(ind1Values);
        values.push_back(ind2Values);
        bool flag = dbs->insertEntryList(_INDUSTRY_TABLE_NAME, fields, values, N, w);
        TS_ASSERT_EQUALS(flag, true);

        auto res = w.exec("SELECT count(1) FROM "+_INDUSTRY_TABLE_NAME
                +" where name = "+ind1Values[0]);
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "1");

        res = w.exec("SELECT count(1) FROM "+_INDUSTRY_TABLE_NAME
                +" where name = "+ind2Values[0]);
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "1");

        res = w.exec("SELECT count(1) FROM "+_INDUSTRY_TABLE_NAME);
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "2");

        w.commit();

        // Tests incorrect fields.
        std::list<string*> values2;
        values2.push_back(new string[3]{"qweasd", "300", "true"});
        flag = dbs->insertEntryList(_INDUSTRY_TABLE_NAME, new string[3]{"name", "fake_field", "is_active"},
                values2, N, w);
        TS_ASSERT_EQUALS(flag, false);
        
        // Tests incorrect values.
        values.clear();
        flag = dbs->insertEntryList(_INDUSTRY_TABLE_NAME, fields, values, N, w);
        TS_ASSERT_EQUALS(flag, false);

        delete[] ind1Values;
        delete[] ind2Values;
    }

    void testInsertEntry()
    {
        auto dbs = DBSingleton::getSingleton();
        const uint32_t N = 3;

        string fields[N] = {"name", "sort", "is_active"};
        string * values = new string[N]{"'Steel'", "'300'", "'true'"};
        bool flag = dbs->insertEntry(_INDUSTRY_TABLE_NAME, fields, values, N);
        TS_ASSERT_EQUALS(flag, true);
        delete[] values;

        values = new string[N]{"Steel", "300", "true"};
        flag = dbs->insertEntry(_INDUSTRY_TABLE_NAME, fields, values, N);
        TS_ASSERT_EQUALS(flag, false);
        delete[] values;
    }

};
