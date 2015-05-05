#include "../include/FieldCoordinates.h"
#include <cassert>
#include <algorithm>
#include <fstream>
#include <split.h>
#include <trim.h>
#include <map>
#include <iostream>
#include <easylogging++.h>

using std::stoi;

FieldCoordinates::FieldCoordinates(const std::string& path) : _FIELDS_NUM(4)
{
    std::ifstream file(path);
    assert(file);

    std::map<std::string, std::pair<uint8_t, uint8_t>> pars;
    std::string buffer;
    while (getline(file, buffer)) {
        // parse key and value.
        auto values = forge::split(buffer, '=');
        assert(values.size() == 2);
        for (auto& item: values) forge::trim(item);
        const std::string KEY = std::move(values.front());
        values.erase(values.begin());

        // parse value as coordinates.
        values = forge::split(std::move(values.front()), ',');
        assert(values.size() == 2);
        for (auto& item: values) forge::trim(item);
        pars[KEY] = std::make_pair(stoi(values.front()), stoi(values.back()));
    }
    file.close();
    try {
        _init(pars.at("category"), pars.at("name"), pars.at("price"),
            pars.at("code"));
    } catch (const std::out_of_range &e) {
        LOG(ERROR) << "file with coordinates doesn't have required field(s) -> "
            << e.what();
        exit(-1);
    }
}

FieldCoordinates::FieldCoordinates(const std::pair<uint8_t, uint8_t> &category,
    const std::pair<uint8_t, uint8_t> &name,
    const std::pair<uint8_t, uint8_t> &code,
    const std::pair<uint8_t, uint8_t> &price) :
    _FIELDS_NUM(4)
{
    _init(category, name, price, code);
}

FieldCoordinates::FieldCoordinates(const FieldCoordinates &value) : 
    _FIELDS_NUM(4)
{
    _init(value.getCategory(), value.getName(), value.getPrice(),
            value.getCode());
}

void FieldCoordinates::_init(
    const std::pair<uint8_t, uint8_t> &category,
    const std::pair<uint8_t, uint8_t> &name,
    const std::pair<uint8_t, uint8_t> &price,
    const std::pair<uint8_t, uint8_t> &code)
{
    _category = category;
    _name = name;
    _code = code;
    _price = price;
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
