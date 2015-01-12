#include <pqxx/pqxx>
#include <list>
#include <vector>

//! Singleton for working with database.
/*!
  Class helps to send queries to DB.
*/
class DBSingleton
{
public:
    static DBSingleton * getSingleton();
    pqxx::connection * getConnection() const;
    std::list<std::vector<std::string>> getTableData(std::string tableName,
            const std::string * fields=0, const size_t fieldsSize = 0);

    //! Method checks an entry.
    /*!
      Method checks existance of an entry by pointed table and key from field
      name and value.

      \param tableName - table of target entry.
      \param fieldName - name of field, which will be used in search.
      \param value - value for searh.
      \param w - work (libpqxx object)
      \return flag of existance.
    */
    bool checkEntry(const std::string &tableName, const std::string &fieldName,
            const std::string &value, pqxx::work &w) const;
    
    //! Method updates an entry.
    /*!
      Method updates entry on pointed table by name, and after this updating
      entry will have pointed values in pointed fields respectively.

      \param tableName - table of target entry.
      \param fields - array of fields for updating. Includes key field.
      \param values - array of values which will be in entry after updating.
      \param fieldsNum - number of fields within key field.
      \param keyFieldName - name of field, which will be used in search.
      \param w - work (libpqxx object)
      \param keyFieldIdx - index of key field in array fields and index of
      key value in values. If will not be pointed, it will be computed.
      \return flag
     */
    bool updateEntry(const std::string &tableName, const std::string fields[],
            const std::string values[], size_t fieldsNum,
            const std::string &keyFieldName, pqxx::work &w,
            size_t keyFieldIdx = 0) const;

    //! Method inserts an entry.
    /*!
      Method updates entry on pointed table by name, and after this updating
      entry will have pointed values in pointed fields respectively.

      \param tableName - table of target entry.
      \param fields - array of fields for updating. Includes key field.
      \param values - array of values which will be in entry after updating.
      \param fieldsNum - number of fields within key field.
      \param w - work (libpqxx object)
    */
    bool insertEntry(const std::string &tableName, const std::string fields[],
            const std::list<std::string *> &values, size_t fieldsNum,
            pqxx::work &w);


private:
    enum QueryType : uint8_t { SELECT=0, UPDATE=1 };
    ///
    /// \brief Method for getting condition. This condition helps to work
    /// with entries of pointed company.
    /// \param tableName - name of table of the target entry.
    /// \param keyField - name of field, which will be used in search.
    /// \param value - value for search.
    /// \return condition as string.
    ///
    std::string _generateCondition(const std::string &tableName,
            const std::string &keyField, const std::string &keyValue,
            pqxx::work &w, const QueryType type) const;

    DBSingleton();
    virtual ~DBSingleton ();
    static DBSingleton * _self;
    pqxx::connection * _connection;
};
