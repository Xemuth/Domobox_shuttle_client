
#include "StateMachine.hpp"
#include "States/Initialisation.hpp"
#include "States/Error.hpp"
#include "esp_wifi.h"
#include <exception>

namespace domobox{

    S_Initialisation::S_Initialisation(){
        printf("Constructor\n");
    }

    S_Initialisation::~S_Initialisation(){
        printf("Destructor\n");
    }
    
    ALL_STATES S_Initialisation::GetName() const{return ALL_STATES::INITIALISATION;}

    void S_Initialisation::Update(const char* subject){
        printf("%s\n", __PRETTY_FUNCTION__);
    }

    std::unique_ptr<DState> S_Initialisation::SetState(ALL_STATES state){
        printf("%s\n", __PRETTY_FUNCTION__);
        return {};
    }

    std::unique_ptr<DState> S_Initialisation::Next(){

        // We init wifi component, if it fail then we go to Error state and reset after notifying alarm via led:


        //For now we are only going to Error state
        return std::unique_ptr<S_Error>(new S_Error);
    }

    esp_err_t S_Initialisation::InitWifi(){
        //init wifi return error code
        try{
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
        }catch(std::exception& e){
            printf(e.what());
        }
        return 0;
    }

}