/********************************************************           
* Defintition -- provide all exception and macro       *
  for error handling.                                  *
*                                                      *   
* Author:  Clément Hamon                               *   
********************************************************/  
#ifndef DEFINITION_HEADER
#define DEFINITION_HEADER
#include <esp_err.h>
#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static std::string _error_message_preparation(esp_err_t rc, const char *file, int line, const char *function, const char *expression){
    char buffer[400];
    sprintf(buffer, "ESP_ERROR %d (%s)\nFile: %s line %d\nFunction: %s\nExpression: %s", rc, esp_err_to_name(rc), file, line, function, expression);
    return std::string(buffer);
}

#define DOMOBOX_CREATE_EXCEPTION(x, exception_type)                                                                         \
    esp_err_t __err_rc = (x);                                                                                               \
    if (__err_rc != ESP_OK) {                                                                                               \
        throw exception_type(std::move(_error_message_preparation(__err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, #x))); \
    }                                                                                                                       \

#define DOMOBOX_FATAL_ERROR_CHECK(x) DOMOBOX_CREATE_EXCEPTION(x, FatalEspException)
#define DOMOBOX_ERROR_CHECK(x) DOMOBOX_CREATE_EXCEPTION(x, EspException)
#define DOMOBOX_EXCEPTION(x) DOMOBOX_CREATE_EXCEPTION(x, DomoboxException)

namespace domobox{
    /** Domobox exception is the exception raised when something 
     *  went wrong during domobox routine.
     */
    class DomoboxException: public std::exception{
        public:
            explicit DomoboxException(const char* message) : msg_(message) {}
            explicit DomoboxException(const std::string& message) : msg_(message) {}
            explicit DomoboxException(std::string&& message) : msg_(std::move(message)) {}
            virtual ~DomoboxException() noexcept {}
            virtual const char* what() const noexcept {return msg_.c_str();}
        protected:
            std::string msg_;
    };

    /** Fatal Esp Exception is the exception raised when something 
     *  went wrong with esp function. Instead of aborting this one 
     *  is raised and allow domobox to set is error state correctly before reseting.
     */
    class FatalEspException: public DomoboxException {};

    /** Esp Exception is the exception raised when something 
     *  is going on esp function. It should allow to recover by 
     *  going to the correct state depending on current state.
     */
    class EspException: public DomoboxException {};
}
    
#endif