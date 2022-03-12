#include "StateMachine.hpp"
#include "States/Error.hpp"
#include <esp_system.h>
namespace domobox{

    S_Error::S_Error(){
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = (1ULL << alarm_led_gpio);
        gpio_config(&io_conf);
    }

    S_Error::~S_Error(){
        printf("Destructor\n");
    }
    
    ALL_STATES S_Error::GetName() const{return ALL_STATES::INITIALISATION;}

    void S_Error::Update(const char* subject){
        printf("%s\n", __PRETTY_FUNCTION__);
    }

    std::unique_ptr<DState> S_Error::SetState(ALL_STATES state){
        printf("%s\n", __PRETTY_FUNCTION__);
        return {};
    }

    std::unique_ptr<DState> S_Error::Next(){
        // in error, we toggle the red led and we reset after 10 times
        static bool alarm_state = false;
        alarm_state = !alarm_state;
        gpio_set_level(alarm_led_gpio, !alarm_state);
        cpt_before_reset++;
        if(cpt_before_reset > max_blink){
            // we reset here
            esp_restart();
        }
        return {};
    }
}