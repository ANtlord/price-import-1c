#include "../include/ConfigSingleton.h"

using std::string;
ConfigSingleton * ConfigSingleton::_self = nullptr;

ConfigSingleton * ConfigSingleton::getSingleton()
{
    if (_self == nullptr)
        _self = new ConfigSingleton();
    return _self;
}

ConfigSingleton::ConfigSingleton ()
{
    // Is only for Linux.
    string path(realpath("/proc/self/exe", NULL));
    _data[PATH] = std::move(path.substr(0, path.rfind('/')));
}

ConfigSingleton * ConfigSingleton::addOption(Options key, const std::string &value)
{
    _data[key]=value;
    return this;
}

const std::string &ConfigSingleton::getOption(Options key) const
{
    return _data.at(key);
}
