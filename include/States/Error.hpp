/*******************************************************
* Error state -- In this state, we print the error     *
* which lead to this state and blink the error led.    *
* The only way to leave this state is to perform reset.*
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/
#ifndef STATE_ERROR_HEADER
#define STATE_ERROR_HEADER
#include <driver/gpio.h>
#include "StateMachine.hpp"

namespace domobox{

    class S_Error: public domobox::DState{
        public:
            S_Error(std::string&& error);
            virtual ~S_Error();

            ALL_STATES GetName() const;
            std::unique_ptr<DState> Next();
        
        private:
            std::string error_msg;
            gpio_config_t io_conf;
            gpio_num_t led = GPIO_NUM_2;
    };

}

#endif