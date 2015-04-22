#include "../include/FieldCoordinates.h"
#include <cassert>
#include <algorithm>

FieldCoordinates::FieldCoordinates(const std::pair<uint8_t, uint8_t> &category,
    const std::pair<uint8_t, uint8_t> &name,
    const std::pair<uint8_t, uint8_t> &code,
    const std::pair<uint8_t, uint8_t> &price)
{
    _category = category;
    _name = name;
    _code = code;
    _price = price;
}

FieldCoordinates::FieldCoordinates(const FieldCoordinates &value)
{
    _category = value.getCategory();
    _name = value.getName();
    _code = value.getCode();
    _price = value.getPrice();
}

uint8_t FieldCoordinates::getLineWidth(uint8_t index) const
{
    assert(index < _FIELDS_NUM);
    uint8_t res = 0;
    const std::pair<uint8_t, uint8_t> * FIELDS[] = {
        &_name, &_code, &_price, &_category
    };

    for (uint8_t i = 0; i < _FIELDS_NUM; ++i)
        if (FIELDS[i]->second == index)
            res = std::max(res, FIELDS[i]->first);

    return res+1;   // Length = last index + 1.
}

ResultIndexes FieldCoordinates::getResultIndex(uint8_t x, uint8_t y) const
{
    const std::pair<uint8_t, uint8_t> * FIELDS[] = {
        &_name, &_code, &_price, &_category
    };
    for (uint8_t i = 0; i < _FIELDS_NUM; ++i)
        if (FIELDS[i]->first == x && FIELDS[i]->second == y)
            return (ResultIndexes)i;
    ResultIndexesException exception;
    throw exception;
}

bool FieldCoordinates::isPrice(uint8_t x, uint8_t y) const
{
    return (_price.first == x && _price.second == y);
}

//uint8_t ** FieldCoordinates::getFieldIndexes() const
//{
    /*
    uint8_t ** res = new uint8_t*[_FIELDS_NUM];
    for (uint8_t i = 0; i < _FIELDS_NUM; ++i) {
        res[i] = new uint8_t*[getLineWidth(i)];

        for (uint8_t j = 0; j < getLineWidth(i); ++j) {
            res[i][j] = 
        }
    }
    return res;
    */ 
//}
