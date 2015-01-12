#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"
#include <join.h>
#include <each.h>
#include <find.h>
#include <functional>
#include <exception>
#include <list>
#include <iostream>

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
        std::list<std::string*> insertData;

        if (_KEY != 0) {
            for (auto it: _data) {
                // update entry.
                if (dbSingleton->checkEntry(_TABLE, _KEY, it[_keyIndex], w)) {
                    dbSingleton->updateEntry(_TABLE, _FIELDS, it, _N, _KEY, w,
                            _keyIndex);
                }
                else {    // insert entry.
                    insertData.push_back(it);
                }
            }
        }

        if (_KEY == 0) {
            insertData = _data;
        }
        dbSingleton->insertEntry(_TABLE, _FIELDS, insertData, _N, w);
        w.commit();
        std::cout << "udpated: " << _data.size() - insertData.size() << std::endl;
        std::cout << "inserted: " << insertData.size() << std::endl;
        return true;
    }
}

size_t SaveCommand::getFieldsLength() const
{
    return _N;
}

void SaveCommand::clearData()
{
    for (auto it: _data) delete[] it;
    _data.clear();
}

void SaveCommand::addData(std::string data[])
{
    _data.push_back(data);
}
