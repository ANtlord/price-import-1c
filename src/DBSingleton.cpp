#include "../include/DBSingleton.h"
#include <cassert>
#include <join.h>
#include <iostream>

using std::string;

DBSingleton * DBSingleton::_self = 0;

DBSingleton::DBSingleton()
{
    _connection = new pqxx::connection("dbname=cpp_base user=postgres");
}

DBSingleton * DBSingleton::getSingleton()
{
    if (_self == 0)
        _self = new DBSingleton();
    return _self;
}

pqxx::connection * DBSingleton::getConnection() const
{
    return this->_connection;
}

DBSingleton::~DBSingleton ()
{

}

std::list<std::vector<string>> DBSingleton::getTableData(string tableName,
        const string fields[], const size_t fieldsSize)
{
    if (fields != 0 || fieldsSize != 0) assert(fieldsSize > 0 && fields != 0);
    pqxx::work w(*this->getConnection());
    string queryString = "select " + ((fields == 0) ? "*" : forge::join(fields, fieldsSize, ","))
        + " from " + tableName;
    pqxx::result res = w.exec(queryString);

    std::list<std::vector<string>> resList;
    for (auto it = res.begin(); it != res.end(); ++it) {
        std::vector<string> vec;
        for (auto sit = it.begin(); sit != it.end(); ++sit) {
            vec.push_back(sit.as<string>());
        }
        resList.push_back(vec);
    }
    w.commit();
    return resList;
}
