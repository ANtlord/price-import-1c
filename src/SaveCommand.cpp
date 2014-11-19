#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"
#include <join.h>

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
        pqxx::work w(*(DBSingleton::getSingleton()->getConnection()));

        std::string queryString = "insert into "+_table+" (";
        queryString += forge::join(_fields, _n, ", ");
        queryString += ") values ";

        size_t i=0;
        for (auto it = _data.begin(); it != _data.end(); ++it) {
            queryString += ("("+forge::join(*it, _n, ", ")+")");
            if (i < _data.size()-1) queryString += ",";
            ++i;
        }

        pqxx::result res = w.exec(queryString);
        return res.size() == 1;
    }
}

size_t SaveCommand::getFieldsLength() const
{
    return _n;
}

void SaveCommand::addData(std::string * data)
{
    _data.push_back(data);
}
