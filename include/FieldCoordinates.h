#ifndef FIELDCOORDINATES_H
#define FIELDCOORDINATES_H
#include <utility>
#include <stdint.h>
#include <exception>
#include <string>
#include <array>

enum ResultIndexes : uint8_t { NAME=0, CODE=1, PRICE=2, SECTION=3 };

class ResultIndexesException : public std::exception
{
public:
    virtual const char* what(uint8_t x, uint8_t y) const throw() {
        std::string str = "Undefined index x: "+std::to_string(x)+" y: "+std::to_string(y);
        return str.c_str();
    }
};


class FieldCoordinates
{
public:
    FieldCoordinates(const std::pair<uint8_t, uint8_t> &category,
        const std::pair<uint8_t, uint8_t> &name,
        const std::pair<uint8_t, uint8_t> &price,
        const std::pair<uint8_t, uint8_t> &code);

    FieldCoordinates(const FieldCoordinates &value);
    virtual ~FieldCoordinates();

    uint8_t getLineWidth(uint8_t index) const;
    ResultIndexes getResultIndex(uint8_t x, uint8_t y) const;
    bool isPrice(uint8_t x, uint8_t y) const;

    const std::pair<uint8_t, uint8_t> &getCategory() const { return _category; }
    const std::pair<uint8_t, uint8_t> &getPrice() const { return _price; }
    const std::pair<uint8_t, uint8_t> &getCode() const { return _code; }
    const std::pair<uint8_t, uint8_t> &getName() const { return _name; }
    const uint8_t &getFieldsNum() const { return _FIELDS_NUM; }

    const std::pair<uint8_t, uint8_t>* const* getFieldsAsArray() const;

private:
    std::pair<uint8_t, uint8_t> _category;
    std::pair<uint8_t, uint8_t> _name;
    std::pair<uint8_t, uint8_t> _price;
    std::pair<uint8_t, uint8_t> _code;
    const uint8_t _FIELDS_NUM;
    const std::pair<uint8_t, uint8_t>* const* _FIELDS;
};

#endif //FIELDCOORDINATES_H
