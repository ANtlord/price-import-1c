#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"
#include <join.h>
#include <each.h>
#include <functional>
#include <exception>

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

using pqxx::work;

bool SaveCommand::execute() const
{
    if (_data.size() == 0) return false;
    else {
        work w(*(DBSingleton::getSingleton()->getConnection()));
        //pqxx::tablewriter writer(w, _table, &_fields[0], &_fields[_n]);

        std::string queryString = "insert into category ";
        queryString += ("("+forge::join(_fields, _n, ", ")+") values ");

        size_t i = 0;
        std::string * arr = 0;
        for (auto it = _data.begin(); it != _data.end(); ++it) {
            forge::each<std::string>([&w](std::string &item){item = w.quote(item);}, *it, _n);
            queryString += ("("+forge::join(*it, _n, ", ")+")");
            if (i < _data.size()-1) {
                queryString += ",";
            }
            ++i;
        }
        w.exec(queryString);
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
    _data.clear();
}

void SaveCommand::addData(std::string data[])
{
    _data.push_back(data);
}
