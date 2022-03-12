#ifndef STATE_ERROR_HEADER
#define STATE_ERROR_HEADER
#include "StateMachine.hpp"
#include <driver/gpio.h>

namespace domobox{

    class S_Error: public domobox::DState{
        public:
            S_Error();
            virtual ~S_Error();

            ALL_STATES GetName() const;
            void Update(const char* subject);
            std::unique_ptr<DState> SetState(ALL_STATES state);
            std::unique_ptr<DState> Next();
        
        private:
            gpio_config_t io_conf;
            uint8_t cpt_before_reset = 0;
            gpio_num_t alarm_led_gpio = GPIO_NUM_2;
            uint8_t max_blink = 20;
    };

}

#endif