
#include "StateMachine.hpp"
#include "States/Initialisation.hpp"
#include "Definition.hpp"
#include "esp_wifi.h"


namespace domobox{

    S_Initialisation::S_Initialisation(){}

    S_Initialisation::~S_Initialisation(){}

    ALL_STATES S_Initialisation::GetName() const{return ALL_STATES::INITIALISATION;}

    std::unique_ptr<DState> S_Initialisation::Next(){
        // We init wifi component, if it fail then we go to Error state and reset after notifying alarm via led:
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        DOMOBOX_FSM_CHECK(esp_wifi_init(&cfg));
        return {};
    }
}