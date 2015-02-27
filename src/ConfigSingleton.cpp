#include "../include/ConfigSingleton.h"

ConfigSingleton * ConfigSingleton::_self = nullptr;

ConfigSingleton * ConfigSingleton::getSingleton()
{
    if (_self == nullptr)
        _self = new ConfigSingleton();
    return _self;
}

ConfigSingleton::ConfigSingleton ()
{

}

ConfigSingleton::~ConfigSingleton ()
{

}

ConfigSingleton * ConfigSingleton::addOption(const std::string &key, const std::string &value)
{
    _data[key]=value;
    return this;
}

const std::string &ConfigSingleton::getOption(const std::string &key) const
{
    return _data.at(key);
}
