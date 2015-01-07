#ifndef CONFIGSINGLETON_H
#define CONFIGSINGLETON_H
#include <map>
#include <string>

class ConfigSingleton
{
public:
    static ConfigSingleton * getSingleton();
    virtual ~ConfigSingleton ();
    ConfigSingleton * addOption(const std::string &key, const std::string &value);
    const std::string &getOption(const std::string &key) const;

private:
    static ConfigSingleton * _self;
    ConfigSingleton();
    std::map<std::string, std::string> _data;
};
#endif //CONFIGSINGLETON_H
