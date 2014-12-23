#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"
#include <join.h>
#include <each.h>
#include <functional>
#include <exception>
#include <list>

using pqxx::work;
using std::list;
SaveCommand::SaveCommand(const char* table, std::string fields[], size_t n,
        const char * key) : _KEY(key), _TABLE(table), _N(n)
{
    _FIELDS = fields;
}

SaveCommand::~SaveCommand()
{
    delete[] _FIELDS;
}

const std::string * SaveCommand::getFields() const
{
    return _FIELDS;
}


bool SaveCommand::execute() const
{
    if (_data.size() == 0) return false;
    else {
        work w(*(DBSingleton::getSingleton()->getConnection()));
        list<std::string*> updateData;
        const size_t IDX = 0;

        std::string insertQueryString = "insert into "+_TABLE+" ";

        if (_KEY != 0) {
            for (auto it = _data.begin(); it != _data.end(); ++it) {
                std::string query = "select exists(select 1 from "+_TABLE
                        +" where "+_KEY+" ilike "+w.quote(*it[IDX])+")";
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
        }

        std::string queryString = "insert into "+_TABLE+" ";
        queryString += ("("+forge::join(_FIELDS, _N, ", ")+") values ");

        size_t i = 0;
        for (auto it = _data.begin(); it != _data.end(); ++it) {
            forge::each<std::string>([&w](std::string &item){
                    item = w.quote(item);}, *it, _N);
            queryString += ("("+forge::join(*it, _N, ", ")+")");
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
    return _N;
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
