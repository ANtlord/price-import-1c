#ifndef CONFIGSINGLETON_H
#define CONFIGSINGLETON_H
#include <string>
#include <array>

enum Options : uint8_t {
    COMPANY_ID = 0,
    DB_NAME = 1,
    DB_USER = 2,
    PRODUCT_TABLE = 3,
    CATEGORY_TABLE = 4
};

class ConfigSingleton
{
public:
    
    static ConfigSingleton * getSingleton();
    ConfigSingleton * addOption(Options key, const std::string &value);
    const std::string &getOption(Options key) const;

private:
    std::array<std::string, 5 /* Options length */> _data;
    static ConfigSingleton * _self;
    ConfigSingleton();
    virtual ~ConfigSingleton ();
};
#endif //CONFIGSINGLETON_H
