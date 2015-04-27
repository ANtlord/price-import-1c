#include "../include/DBSingleton.h"
#include "../include/ConfigSingleton.h"
#include <cassert>
#include <join.h>
#include <iostream>
#include <find.h>
#include <each.h>
#include <easylogging++.h>

using std::string;
using std::vector;

DBSingleton * DBSingleton::_self = nullptr;

DBSingleton::DBSingleton()
{
    auto config = ConfigSingleton::getSingleton();
    _connection = new pqxx::connection("dbname="+config->getOption(Options::DB_NAME)
            +" user="+config->getOption(Options::DB_USER));
}

DBSingleton * DBSingleton::getSingleton()
{
    if (_self == nullptr)
        _self = new DBSingleton();
    return _self;
}

pqxx::connection * DBSingleton::getConnection() const
{
    return this->_connection;
}

std::list<std::vector<string>> DBSingleton::getTableData(string tableName,
        const string fields[], const size_t fieldsSize)
{
    // Gets data.
    if (fields != 0 || fieldsSize != 0) assert(fieldsSize > 0 && fields != 0);
    pqxx::work w(*this->getConnection());
    string queryString = "select " + ((fields == 0) ? "*" : forge::join(fields,
                fieldsSize, ",")) + " from " + tableName
        + " where section_id = " + ConfigSingleton::getSingleton()->getOption(Options::COMPANY_ID);
    pqxx::result res = w.exec(queryString);

    // Parses data.
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
    string query = "select exists(select 1 from "+tableName+" as t1"
        +_generateCondition(tableName, fieldName, value, w, SELECT)+")";
    const string ERROR_STRING = "Error while checking existance of " + value;

    pqxx::result res;
    bool execRes = _execWork(w, query, &res);
    LOG_IF(!execRes, ERROR) << ERROR_STRING;
    assert(execRes);
    if (res.size() != 1) {
        LOG(ERROR) << ERROR_STRING;
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
        if (keyFieldIdx == SIZE_MAX) {
            // Computes index of key field.
            uint32_t i=0;
            while (i < fieldsNum && keyFieldName != fields[i]) ++i;
            keyFieldIdx = i;
        }

        if (keyFieldIdx != SIZE_MAX && keyFieldIdx < fieldsNum) {
            string arr[fieldsNum-1];
            size_t index;
            for (size_t i = 0; i < fieldsNum-1; ++i) {
                index = (i < keyFieldIdx) ? i : i+1;
                arr[i] = fields[index] + "=" + w.quote(values[index]);
            }
            std::string pars = forge::join(arr, (fieldsNum-1), ", ");
            std::string query = "update "+tableName+" as t1 set "+pars
                +_generateCondition(tableName, keyFieldName,
                        values[keyFieldIdx], w, UPDATE);

            bool res = _execWork(w, query);
            LOG_IF(!res, ERROR) << "Error in update query: " << query;
            return res;
        }
        else {
            LOG(ERROR) << "Key field index is greater than number of fields: "
                << keyFieldIdx << " > " << fieldsNum;
            return false;
        }
    }
    return false;
}

bool DBSingleton::insertEntryList(const std::string &tableName,
        const std::string fields[], const std::list<std::string *> &values,
        const size_t fieldsNum, pqxx::work &w)
{
    std::string query = "insert into "+tableName+" " 
        + ("("+forge::join(fields, fieldsNum, ", ")+") values ");
    if (values.size() > 0) {
        size_t i=0;

        for (auto it: values) {
            forge::each<std::string>([&w](std::string &item){
                    item = w.quote(item);}, it, fieldsNum);
            query += ("("+forge::join(it, fieldsNum, ", ")+")");
            if (i < values.size()-1) {
                query += ",";
            }
            ++i;
        }
        return _execWork(w, query);
    }
    return false;
}

bool DBSingleton::insertEntry(const std::string &tableName, const string fields[],
        const string values[], size_t fieldsNum)
{
    pqxx::work w(*getConnection());
    std::string query = "insert into "+tableName+" " 
        + ("("+forge::join(fields, fieldsNum, ", ")+") values ")
        + ("("+forge::join(values, fieldsNum, ", ")+")");

    bool res = _execWork(w, query);
    if (res) w.commit();
    else LOG(ERROR) << "Error in insert query: " << query.c_str();
    return res;
}

bool DBSingleton::_execWork(pqxx::work &w, const string& query, pqxx::result * res) const
{
    const string ERROR_PREFIX = "Error query: " + query;
    try {
        if (res != nullptr) *res = w.exec(query);
        else w.exec(query);
        return true;
    }
    catch (pqxx::data_exception &e) {
        LOG(ERROR) << ERROR_PREFIX << " " << e.what();
        return false;
    }
    catch (pqxx::undefined_column &e) {
        LOG(ERROR) << ERROR_PREFIX << " " << e.what();
        return false;
    }
    catch (pqxx::usage_error &e) {
        LOG(ERROR) << ERROR_PREFIX << " " << e.what();
        return false;
    }
}

std::string DBSingleton::_generateCondition(const std::string &tableName,
        const std::string &keyField, const std::string &keyValue,
        pqxx::work &w, const QueryType type) const
{
    auto config = ConfigSingleton::getSingleton();
    std::string res;
    const bool FLAG = (type == SELECT);

    if (config->getOption(Options::PRODUCT_TABLE) == tableName){
        res = (FLAG) ? " left join " : " from ";
        res += config->getOption(Options::CATEGORY_TABLE)+" as t2"
            + ((FLAG) ? " on" : " where")
            +" t1.section_id = t2.id";
    }

    if (res.find("where") == string::npos) {
        res += string(" where") + " t1."+keyField+" = "
            + w.quote(keyValue) +" and ";
    }
    else {
        res += string((FLAG) ? " where" : " and")+" t1."+keyField+" = "
            + w.quote(keyValue) +" and ";
    }

    if (config->getOption(Options::PRODUCT_TABLE) == tableName) res += "t2";
    else res += "t1";

    res += ".section_id = " + config->getOption(Options::COMPANY_ID);
    return res;
}
