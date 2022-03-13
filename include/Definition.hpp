/*******************************************************           
* Defintition -- provide few features and macro for    *
* error handling.                                      *
*                                                      *   
* Author:  Clément Hamon                               *   
********************************************************/  
#ifndef DEFINITION_HEADER
#define DEFINITION_HEADER
#include <esp_err.h>
#include <string>
#include "States/Error.hpp"

static std::string _error_message_preparation(esp_err_t rc, const char *file, int line, const char *function, const char *expression){
    char buffer[400];
    sprintf(buffer, "ESP_ERROR %d (%s)\nFile: %s line %d\nFunction: %s\nExpression: %s\n%s\n", rc, esp_err_to_name(rc), file, line, function, expression,
    "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
    return std::string(buffer);
}

/*
    We check for error on ESP function. If wrong we move to Error state and print message until reset
*/
#define DOMOBOX_FSM_CHECK(x){                                                                                                                       \
    esp_err_t __err_rc = (x);                                                                                                                       \
    if (__err_rc != ESP_OK) {                                                                                                                       \
        return std::unique_ptr<S_Error>(new S_Error(std::move(_error_message_preparation(__err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, #x)))); \
    }                                                                                                                                               \
}
#endif