/*******************************************************
* Defintition -- provide few features and macro for    *
* error handling. Configuration management. Etc...     *
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/
#include "Definition.hpp"

namespace domobox{

DomoboxConfiguration *DomoboxConfiguration::instance = nullptr;

DomoboxConfiguration& DomoboxConfiguration::Get(){
    if(!instance)
        instance = new DomoboxConfiguration;
    return *instance;
}

std::string _error_message_preparation(esp_err_t rc, const char *file, int line, const char *function, const char *expression){
    char buffer[400];
    sprintf(buffer, "ESP_ERROR %d (%s)\nFile: %s line %d\nFunction: %s\nExpression: %s\n%s\n", rc, esp_err_to_name(rc), file, line, function, expression,
    "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
    return std::string(buffer);
}

std::string _assert_message_preparation(const char* message_assert, const char *file, int line, const char *function, const char *expression){
    char buffer[400];
    sprintf(buffer, "DOMOBOX_ASSERT %s\nFile: %s line %d\nFunction: %s\nExpression: %s\n%s\n", message_assert, file, line, function, expression,
    "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
    return std::string(buffer);
}

}