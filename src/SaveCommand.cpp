#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"
#include <join.h>
#include <each.h>
#include <functional>
#include <exception>
#include <list>

using pqxx::work;
using std::list;
SaveCommand::SaveCommand(const char* table, std::string fields[], size_t n)
{
    _fields = fields;
    _n = n;
    _table = std::string(table);
}

SaveCommand::SaveCommand(){}

SaveCommand::~SaveCommand()
{
    delete[] _fields;
}

std::string * SaveCommand::getFields() const
{
    return _fields;
}


bool SaveCommand::execute() const
{
    if (_data.size() == 0) return false;
    else {
        work w(*(DBSingleton::getSingleton()->getConnection()));
        list<std::string*> updateData;
        const std::string UNIQUE_FIELD = "name";
        const size_t IDX = 0;

        std::string insertQueryString = "insert into "+_table+" ";

        for (auto it = _data.begin(); it != _data.end(); ++it) {
            std::string query = "select exists(select 1 from "+_table
                    +" where "+UNIQUE_FIELD+" ilike "+w.quote(*it[IDX])+")";
            pqxx::result res;
            
            try {
                res = w.exec(query);
            }
            catch (pqxx::data_exception &e) {
                std::cout << "Error query: " << query << std::endl;
                return false;
            }
            if (res.size() != 1) {
                std::cout << "Error while checking existance of "<< it[IDX] << std::endl;
                return false;
            }
            else if (res[0][0].as<bool>()) {    // update entry.
                updateData.push_back(*it);
            }
            else {  // insert entry.

            }
        }

        std::string queryString = "insert into "+_table+" ";
        queryString += ("("+forge::join(_fields, _n, ", ")+") values ");

        size_t i = 0;
        for (auto it = _data.begin(); it != _data.end(); ++it) {
            forge::each<std::string>([&w](std::string &item){
                    item = w.quote(item);}, *it, _n);
            queryString += ("("+forge::join(*it, _n, ", ")+")");
            if (i < _data.size()-1) {
                queryString += ",";
            }
            ++i;
        }
        try {
            w.exec(queryString);
        }
        catch (pqxx::data_exception &e) {
            std::cout << "Error query: " << queryString << std::endl;
        }
        w.commit();
        return true;
    }
}

size_t SaveCommand::getFieldsLength() const
{
    return _n;
}

void SaveCommand::clearData()
{
    for (auto it = _data.begin(); it != _data.end(); ++it) delete[] *it;
    _data.clear();
}

void SaveCommand::addData(std::string data[])
{
    _data.push_back(data);
}
