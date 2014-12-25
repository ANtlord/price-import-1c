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
    if (_KEY != 0) {
        for (size_t i = 0; i < _N; ++i) {
            if (_FIELDS[i] == _KEY) {
                _keyIndex = i;
                i=_N;
            }
        }
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
        work w(*(DBSingleton::getSingleton()->getConnection()));
        list<std::string*> updateData;
        std::vector<std::string*> insertData;

        std::string insertQueryString = "insert into "+_TABLE+" ";

        if (_KEY != 0) {
            for (auto it = _data.begin(); it != _data.end(); ++it) {
                std::string query = "select exists(select 1 from "+_TABLE
                        +" where "+_KEY+" = "+w.quote((*it)[_keyIndex])+")";
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
                        << (*it)[_keyIndex] << std::endl;
                    return false;
                }
                else if (res[0][0].as<bool>()) {    // update entry.
                    //updateData.push_back(*it);
                    if (_N > 1) {   // If we have another fields, except key field.
                        std::string arr[_N-1];
                        size_t index;
                        for (size_t i = 0; i < _N-1; ++i) {
                            index = (i < _keyIndex) ? i : i+1;
                            arr[i] = _FIELDS[index] + "=" + w.quote((*it)[index]);
                        }
                        std::string pars = forge::join(arr, (_N-1), ", ");
                        w.exec("update "+_TABLE+" set "+pars+" where "+_KEY+"="
                                +w.quote((*it)[_keyIndex]));
                    }
                }
                else if (!res[0][0].as<bool>()) {    // insert entry.
                    insertData.push_back(*it);
                }
                else { // Something just is wrong over there. 
                     //TODO: handle exception.
                }
            }
        }

        std::string queryString = "insert into "+_TABLE+" ";
        queryString += ("("+forge::join(_FIELDS, _N, ", ")+") values ");

        size_t i = 0;
        //if (insertData.size() == 0 && updateData.size() == 0) {
        if (_KEY == 0) {
            insertData = _data;
        }
        if (insertData.size() > 0) {
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
