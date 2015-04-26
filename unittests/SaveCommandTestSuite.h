#include <cxxtest/TestSuite.h>
#include <string>
#include "../include/SaveCommand.h" 
#include <exception>
#include <each.h>
#include "../include/ConfigSingleton.h"
#include "../include/DBSingleton.h"
#include <pqxx/pqxx>

using std::string;
using pqxx::work;
using pqxx::result;

class SaveCommandTestSuite : public CxxTest::TestSuite
{
    const size_t _N = 4;
    string * _fields = new string[_N]{
        "name", "section_id", "sort", "is_active"
    };
    SaveCommand _buildCmd(const string& tableName, const char * key=nullptr)
    {
        string * fields = new string[_N];
        for (size_t i = 0; i < _N; ++i) fields[i] = _fields[i];
        if (key == nullptr) {
            return SaveCommand(tableName.c_str(), fields, _N);
        }
        else {
            return SaveCommand(tableName.c_str(), fields, _N, key);
        }
    }

    SaveCommand _buildCmd(const string& tableName, string * fields, size_t n)
    {
        return SaveCommand(tableName.c_str(), fields, n);
    }

    const string _INDUSTRY_TABLE_NAME = "service_industry";
    const string _CATEGORY_TABLE_NAME = "service_category";
    const string _PRODUCT_TABLE_NAME = "service_product";
    const string _COMPANY_TABLE_NAME = "service_company";
    
public:
    SaveCommandTestSuite()
    {
        ConfigSingleton::getSingleton()
            ->addOption(Options::COMPANY_ID, "1")
            ->addOption(Options::DB_NAME, "test_base_price_import")
            ->addOption(Options::DB_USER, "postgres")
            ->addOption(Options::PRODUCT_TABLE, _PRODUCT_TABLE_NAME)
            ->addOption(Options::CATEGORY_TABLE, _CATEGORY_TABLE_NAME);
    }

    ~SaveCommandTestSuite()
    {
        work w(*(DBSingleton::getSingleton()->getConnection()));
        w.exec("delete from "+_INDUSTRY_TABLE_NAME);
        w.exec("delete from "+_COMPANY_TABLE_NAME);
        w.exec("delete from "+_CATEGORY_TABLE_NAME);
        w.exec("delete from "+_PRODUCT_TABLE_NAME);
        w.commit();
    }

    // It is calls before evere invoking for testName method.
    void setUp()
    {
    }

    void testConstructor()
    {
        size_t const N = 4;
        string * fields = new string[N]{
            "name", "section_id", "sort", "is_active"
        };
        auto cmd = _buildCmd("product");
        TS_ASSERT_EQUALS(cmd.getFieldsLength(), N);
        for (size_t i = 0; i < N; ++i) {
            TS_ASSERT_EQUALS(cmd.getFields()[i], fields[i]);
        }
    }

    void testClearData()
    {
        // TODO: make interface for getting fields "data" in SaveCommand class.
        auto cmd = _buildCmd("product", "name");
        cmd.addData(new string[_N]{"pen", "1", "300", "true"});
        cmd.clearData();
        TS_ASSERT_EQUALS(cmd.getData()->size(), 0);
    }

    void testAddData()
    {
        auto cmd = _buildCmd("product");
        auto data = new string[_N]{"pen", "1", "300", "true"};
        cmd.addData(data);
        TS_ASSERT_EQUALS(cmd.getData()->size(), 1);

        // first loop is need, because we can't get item from list by index.
        // But we know, that it has only one item.
        for (const auto &item : *cmd.getData()) {
            for (size_t i = 0; i < _N; ++i) {
                TS_ASSERT_EQUALS(item[i], data[i]);
            }
        }
    }

    void testExecute()
    {
        // Test insert data.
        work w(*(DBSingleton::getSingleton()->getConnection()));
        result res = w.exec("select count(1) from "+_INDUSTRY_TABLE_NAME);
        w.commit();
        uint64_t count = res[0][0].as<uint64_t>();

        uint8_t N = 3;
        auto cmd = _buildCmd(_INDUSTRY_TABLE_NAME, new string[N]{"name",
                 "sort", "is_active"}, N);
        cmd.addData(new string[N]{"super_ind", "300", "true"});
        cmd.execute();

        work w2(*(DBSingleton::getSingleton()->getConnection()));
        res = w2.exec("select count(1) from "+_INDUSTRY_TABLE_NAME);
        TS_ASSERT_EQUALS(res[0][0].as<uint64_t>(), count+1);

        // Test update data.
        w2.exec(" insert into "+_COMPANY_TABLE_NAME
            +" (id, sort, name, short_text, created_at, is_active)"
            +" VALUES ("+ConfigSingleton::getSingleton()->getOption(Options::COMPANY_ID)
            +", 300, 'Toy World', 'The best toys', '2013-01-01', true)");

        res = w2.exec("select id from "+_COMPANY_TABLE_NAME+" where name = 'Toy World'");
        const string COMPANY_ID = res[0][0].as<string>();
        TS_ASSERT_EQUALS( COMPANY_ID, ConfigSingleton::getSingleton()
                ->getOption(Options::COMPANY_ID));
        w2.exec("insert into "+_CATEGORY_TABLE_NAME
            +" (sort, name, is_active, section_id)"
            +" VALUES (300, 'New Toys', true, "+COMPANY_ID+")");

        res = w2.exec("select id from "+_CATEGORY_TABLE_NAME+" where name = 'New Toys'");
        const string CATEGORY_ID = res[0][0].as<string>();
        w2.exec("insert into "+_PRODUCT_TABLE_NAME
            +" (sort, name, is_active, section_id, code, price)"
            +" VALUES (300, 'Toy Iron Man', true, "+CATEGORY_ID+", '00111222', 10)");
        w2.commit();

        const uint8_t PRODUCT_N = 6;
        SaveCommand productCmd(_PRODUCT_TABLE_NAME.c_str(), new string[PRODUCT_N]{
            "name", "code", "price", "section_id", "sort", "is_active"
        }, PRODUCT_N, "code");
        productCmd.addData(new string[PRODUCT_N]{
            "Toy Flash", "00111222", "10.0", CATEGORY_ID, "300", "true"
        });
        productCmd.execute();

        work w3(*(DBSingleton::getSingleton()->getConnection()));
        res = w3.exec("select name from "+_PRODUCT_TABLE_NAME+" where code = '00111222'");
        TS_ASSERT_EQUALS(res[0][0].as<string>(), "Toy Flash");
        w3.commit();
    }
};
