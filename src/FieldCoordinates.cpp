#include "../include/FieldCoordinates.h"
#include <cassert>
#include <algorithm>
#include <iostream>

FieldCoordinates::FieldCoordinates(const std::pair<uint8_t, uint8_t> &category,
    const std::pair<uint8_t, uint8_t> &name,
    const std::pair<uint8_t, uint8_t> &code,
    const std::pair<uint8_t, uint8_t> &price) :
    _FIELDS_NUM(4)
{
    _category = category;
    _name = name;
    _code = code;
    _price = price;
    _FIELDS = new const std::pair<uint8_t, uint8_t>* const[_FIELDS_NUM]{
        &_name, &_code, &_price, &_category
    };
}

FieldCoordinates::FieldCoordinates(const FieldCoordinates &value) : 
    _FIELDS_NUM(4)
{
    _category = value.getCategory();
    _name = value.getName();
    _code = value.getCode();
    _price = value.getPrice();
    _FIELDS = new const std::pair<uint8_t, uint8_t>* const[_FIELDS_NUM]{
        &_name, &_code, &_price, &_category
    };
}

FieldCoordinates::~FieldCoordinates()
{
    delete[] _FIELDS;
}

uint8_t FieldCoordinates::getLineWidth(uint8_t index) const
{
    assert(index < _FIELDS_NUM);
    uint8_t res = 0;

    for (uint8_t i = 0; i < _FIELDS_NUM; ++i)
        if (_FIELDS[i]->second == index)
            res = std::max(res, _FIELDS[i]->first);

    return res+1;   // Length = last index + 1.
}

ResultIndexes FieldCoordinates::getResultIndex(uint8_t x, uint8_t y) const
{
    for (uint8_t i = 0; i < _FIELDS_NUM; ++i)
        if (_FIELDS[i]->first == x && _FIELDS[i]->second == y)
            return (ResultIndexes)i;
    ResultIndexesException exception;
    throw exception;
}

bool FieldCoordinates::isPrice(uint8_t x, uint8_t y) const
{
    return (_price.first == x && _price.second == y);
}

const std::pair<uint8_t, uint8_t>* const* FieldCoordinates::getFieldsAsArray() const
{
    return _FIELDS;
}
