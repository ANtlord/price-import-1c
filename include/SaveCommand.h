#ifndef SAVECOMMAND_H
#define SAVECOMMAND_H
#include <string>
#include <vector>
class SaveCommand
{
public:
    SaveCommand(const char*  table, std::string * fields, size_t n);
    //SaveCommand(std::string * data);
    SaveCommand();
    size_t getFieldsLength() const;
    std::string * getFields() const;
    virtual ~SaveCommand();
    void execute() const;
    void addData(std::string * data);

private:
    std::vector <std::string *> _data;
    std::string * _fields;
    size_t _n;
    std::string _table;
};

#endif //SAVECOMMAND_H
