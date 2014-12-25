#ifndef SAVECOMMAND_H
#define SAVECOMMAND_H
#include <string>
#include <vector>
class SaveCommand
{
public:
    /**
        @table - name of table.
        @fields - array of names of the fields.
        @n - size of fields array.
        @key - unrequired field. It will be user for checking existance of entry.
     */
    SaveCommand(const char*  table, std::string fields[], size_t n,
            const char * key=0);
    size_t getFieldsLength() const;
    const std::string * getFields() const;
    virtual ~SaveCommand();
    bool execute() const;
    void addData(std::string data[]);
    void clearData();

private:
    std::vector <std::string *> _data;
    const std::string * _FIELDS;
    const size_t _N;
    const std::string _TABLE;
    const char * _KEY;
    uint8_t _keyIndex;
};

#endif //SAVECOMMAND_H
