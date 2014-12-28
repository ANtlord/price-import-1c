#include "../include/DBSingleton.h"
#include <cassert>
#include <join.h>
#include <iostream>
#include <find.h>

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
    string queryString = "select " + ((fields == 0) ? "*" : forge::join(fields,
                fieldsSize, ",")) + " from " + tableName;
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

bool DBSingleton::checkEntry(const string &tableName, const string &fieldName,
        const string &value, pqxx::work &w) const
{
    string query = "select exists(select 1 from "+tableName+" where "+fieldName
        +" = "+w.quote(value)+")";

    pqxx::result res;
    try {
        res = w.exec(query);
    }
    catch (pqxx::data_exception &e) {
        std::cout << "Error query: " << query << std::endl;
        return false;
    }
    if (res.size() != 1) {
        std::cout << "Error while checking existance of "
            << value << std::endl;
        return false;
    }
    else { // Something just is wrong over there. 
         //TODO: handle exception.
    }
    return res[0][0].as<bool>();
}

bool DBSingleton::updateEntry(const std::string &tableName,
        const std::string fields[], const std::string values[], size_t fieldsNum,
        const std::string &keyFieldName, pqxx::work &w, size_t keyFieldIdx) const
{
    if (fieldsNum > 1) {   // If we have another fields, except key field.
        if (keyFieldIdx == 0) {
            keyFieldIdx = forge::findFirst<std::string>(
                    [&keyFieldName](const std::string &v) -> bool {
                    return v == keyFieldName;}, fields, fieldsNum);
        } 

        if (keyFieldIdx != 0) {
            std::string arr[fieldsNum-1];
            size_t index;
            for (size_t i = 0; i < fieldsNum-1; ++i) {
                index = (i < keyFieldIdx) ? i : i+1;
                arr[i] = fields[index] + "=" + w.quote(values[index]);
            }
            std::string pars = forge::join(arr, (fieldsNum-1), ", ");
            w.exec("update "+tableName+" set "+pars+" where "+keyFieldName+"="
                    +w.quote(values[keyFieldIdx]));
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}
