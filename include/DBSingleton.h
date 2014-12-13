#include <pqxx/pqxx>
#include <list>

class DBSingleton
{
public:
    static DBSingleton * getSingleton();
    pqxx::connection * getConnection() const;
    std::list<std::vector<std::string>> getTableData(std::string tableName,
            const std::string * fields=0, const size_t fieldsSize = 0);

private:
    DBSingleton();
    virtual ~DBSingleton ();
    static DBSingleton * _self;
    pqxx::connection * _connection;
};
