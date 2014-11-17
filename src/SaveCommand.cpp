#include "../include/SaveCommand.h"

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

void SaveCommand::execute() const
{
}

size_t SaveCommand::getFieldsLength() const
{
    return _n;
}

void SaveCommand::addData(std::string * data)
{
    _data.push_back(data);
}
