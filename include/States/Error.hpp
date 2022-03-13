/*******************************************************
* Error state -- In this state, we print the error     *
* which lead to this state and blink the error led.    *
* The only way to leave this state is to perform reset.*
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/
#ifndef STATE_ERROR_HEADER
#define STATE_ERROR_HEADER
#include "StateMachine.hpp"
#include <driver/gpio.h>

namespace domobox{

    class S_Error: public domobox::DState{
        public:
            S_Error(std::string&& error);
            virtual ~S_Error();

            ALL_STATES GetName() const;
            void Update(const char* subject);
            std::unique_ptr<DState> SetState(ALL_STATES state);
            std::unique_ptr<DState> Next();
        
        private:
            std::string error_msg;
            gpio_config_t io_conf;
            gpio_num_t alarm_led_gpio = GPIO_NUM_2;
    };

}

#endif