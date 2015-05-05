#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"
#include <join.h>
#include <each.h>
#include <find.h>
#include <functional>
#include <exception>
#include <list>
#include <iostream>
#include <easylogging++.h>

using pqxx::work;
using std::list;
SaveCommand::SaveCommand(const char* table, std::string fields[], size_t n,
        const char * key) : _N(n), _TABLE(table), _KEY(key)
{
    _FIELDS = fields;
    if (_KEY != nullptr) {
        _keyIndex = forge::findFirst<std::string>([&key](const std::string &v)
            -> bool {return v == key;}, _FIELDS, _N);
    }
    else _keyIndex=0;
}

SaveCommand::~SaveCommand()
{
    clearData();
    delete[] _FIELDS;
}

const std::string * SaveCommand::getFields() const
{
    return _FIELDS;
}

bool SaveCommand::execute() const
{
    uint64_t insertedCounter = 0, updateErrorCounter = 0;

    if (_data.size() == 0) return false;
    else {
        DBSingleton * dbSingleton = DBSingleton::getSingleton();
        work w(*(dbSingleton->getConnection()));
        std::list<std::string*> insertData;

        if (_KEY != nullptr) {
            for (auto it: _data) {
                // update entry.
                if (dbSingleton->checkEntry(_TABLE, _KEY, it[_keyIndex], w)) {
                    if(!dbSingleton->updateEntry(_TABLE, _FIELDS, it, _N, _KEY,
                                w, _keyIndex))
                    {
                        ++updateErrorCounter;
                    }
                }
                else {    // insert entry.
                    insertData.push_back(it);
                }
            }
        }

        if (_KEY == nullptr) insertData = _data;

        if (insertData.size() > 0) {
            if (dbSingleton->insertEntryList(_TABLE, _FIELDS, insertData, _N, w)) {
                insertedCounter = insertData.size();
                w.commit();
            }
            else { // If query will be fail, than program tries insert items 1 by 1.
                for (auto it: insertData)
                    if (dbSingleton->insertEntry(_TABLE, _FIELDS, it, _N))
                        ++insertedCounter;
            }
        }
        else w.commit();

        LOG(INFO) << "udpated: " << _data.size() - insertData.size() - updateErrorCounter;
        LOG(INFO) << "inserted: " << insertedCounter;
        LOG_IF(updateErrorCounter > 0, WARNING) << "Update Errors: "
            << updateErrorCounter;
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

const std::list <std::string*> * SaveCommand::getData() const
{
    return &_data;
}
