#include "../include/SaveCommand.h"
#include "../include/DBSingleton.h"

SaveCommand::SaveCommand(const char* table, std::string * fields, size_t n)
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
        return true:
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
