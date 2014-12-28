#include <pqxx/pqxx>
#include <list>

class DBSingleton
{
public:
    static DBSingleton * getSingleton();
    pqxx::connection * getConnection() const;
    std::list<std::vector<std::string>> getTableData(std::string tableName,
            const std::string * fields=0, const size_t fieldsSize = 0);
    bool checkEntry(const std::string &tableName, const std::string &fieldName,
            const std::string &value, pqxx::work &w);
    bool updateEntry(const std::string &tableName, const std::string fields[],
            const std::string values[], size_t fieldsNum,
            const std::string &keyFieldName, pqxx::work &w, size_t keyFieldIdx = 0);

private:
    DBSingleton();
    virtual ~DBSingleton ();
    static DBSingleton * _self;
    pqxx::connection * _connection;
};
