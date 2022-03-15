/*******************************************************
* Defintition -- provide few features and macro for    *
* error handling. Configuration management. Etc...     *
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/
#ifndef DEFINITION_HEADER
#define DEFINITION_HEADER
#include <esp_err.h>
#include <string>
#include "States/Error.hpp"

/*
    We check for error on ESP function. If wrong we move to Error state and print message until reset
*/
#define DOMOBOX_FSM_CHECK(function){                                                                                                                           \
    esp_err_t __err_rc = (function);                                                                                                                           \
    if (__err_rc != ESP_OK) {                                                                                                                                  \
        return std::unique_ptr<S_Error>(new S_Error(std::move(_error_message_preparation(__err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, #function))));     \
    }                                                                                                                                                          \
}
#define DOMOBOX_FSM_ASSERT(assert, message_assert){                                                                                                            \
    if(!(assert)){                                                                                                                                             \
        return std::unique_ptr<S_Error>(new S_Error(std::move(_assert_message_preparation(message_assert, __FILE__, __LINE__, __PRETTY_FUNCTION__, #assert))));\
    }                                                                                                                                                          \
}       

namespace domobox{

std::string _error_message_preparation(esp_err_t rc, const char *file, int line, const char *function, const char *expression);
std::string _assert_message_preparation(const char* message_assert, const char *file, int line, const char *function, const char *expression);

class DomoboxConfiguration{
    public:
        static DomoboxConfiguration& Get();
        // All the configuration of domobox is stored in this singleton:
        bool has_credential = false;
        bool is_connected = false;
        bool is_error = false;
        char* ssid[33] = {0};
        char* password[65] = {0};
        std::string error;
    private:
        static DomoboxConfiguration* instance;
};

}

#endif

