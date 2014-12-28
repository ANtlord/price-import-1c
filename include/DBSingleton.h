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
            const std::string &value, pqxx::work &w) const;

    /**
       Updates entry by pointed arguments.
       @tableName - table of target entry.
       @fields - array of fields for updating. Includes key field.
       @values - array of values which will be in entry after updating.
       @fieldsNum - number of fields within key field.
       @keyFieldName - name of field, which will be used in search.
       @w - work (libpqxx object)
       @keyFieldIdx - index of key field in array fields. If will not be pointed,
       it will be computed.
     */
    bool updateEntry(const std::string &tableName, const std::string fields[],
            const std::string values[], size_t fieldsNum,
            const std::string &keyFieldName, pqxx::work &w,
            size_t keyFieldIdx = 0) const;

private:
    DBSingleton();
    virtual ~DBSingleton ();
    static DBSingleton * _self;
    pqxx::connection * _connection;
};
