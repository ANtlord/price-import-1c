#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"
#include <join.h>
#include <each.h>
#include <find.h>
#include <functional>
#include <exception>
#include <list>

using pqxx::work;
using std::list;
SaveCommand::SaveCommand(const char* table, std::string fields[], size_t n,
        const char * key) : _N(n), _TABLE(table), _KEY(key)
{
    _FIELDS = fields;
    if (_KEY != 0) {
        _keyIndex = forge::findFirst<std::string>([&key](const std::string &v)
            -> bool {return v == key;}, _FIELDS, _N);
    }
    else _keyIndex=0;
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
        DBSingleton * dbSingleton = DBSingleton::getSingleton();
        work w(*(dbSingleton->getConnection()));
        std::vector<std::string*> insertData;

        std::string insertQueryString = "insert into "+_TABLE+" ";

        if (_KEY != 0) {
            for (auto it = _data.begin(); it != _data.end(); ++it) {
                bool entryIsExist = dbSingleton->checkEntry(_TABLE, _KEY,
                        (*it)[_keyIndex], w);
                
                if (entryIsExist) {    // update entry.
                    dbSingleton->updateEntry(_TABLE, _FIELDS, (*it), _N, _KEY,
                            w, _keyIndex);
                }
                else {    // insert entry.
                    insertData.push_back(*it);
                }
            }
        }

        std::string queryString = "insert into "+_TABLE+" ";
        queryString += ("("+forge::join(_FIELDS, _N, ", ")+") values ");

        if (_KEY == 0) {
            insertData = _data;
        }
        if (insertData.size() > 0) {
            size_t i = 0;
            for (auto it = insertData.begin(); it != insertData.end(); ++it) {
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
