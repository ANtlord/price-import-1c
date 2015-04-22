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
        w2.commit();
        TS_ASSERT_EQUALS(res[0][0].as<uint64_t>(), count+1);
    }
};
