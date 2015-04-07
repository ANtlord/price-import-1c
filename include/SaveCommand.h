#ifndef SAVECOMMAND_H
#define SAVECOMMAND_H
#include <string>
#include <vector>
#include <list>
class SaveCommand
{
public:
    //! Constructor.
    /*!
      \param table - name of table.
      \param fields - array of names of the fields.
      \param n - size of fields array.
      \param key - unrequired field. It will be user for checking existance of entry.
    */
    SaveCommand(const char*  table, std::string fields[], size_t n,
            const char * key=nullptr);
    size_t getFieldsLength() const;
    const std::string * getFields() const;
    virtual ~SaveCommand();
    bool execute() const;
    void addData(std::string data[]);
    void clearData();
    const std::list <std::string*> * getData() const;

private:
    std::list <std::string *> _data;
    const std::string * _FIELDS;
    const size_t _N;
    const std::string _TABLE;
    const char * _KEY;
    uint8_t _keyIndex;
};

#endif //SAVECOMMAND_H
