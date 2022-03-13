/********************************************************           
* Main -- Entry point of dombox project                *
*                                                      *   
* Author:  Clément Hamon                               *   
********************************************************/  
#include "sdkconfig.h"
#include "StateMachine.hpp"
#include "Definition.hpp"
#include <esp_err.h>

extern "C" {
    void app_main(void);
}

esp_err_t test(){
    return ESP_ERR_FLASH_BASE;
}

using namespace domobox;

void app_main() 
{
    try{
        DOMOBOX_EXCEPTION(test());
    }catch(const DomoboxException& e){
        printf("Catched exception, printing it !\n");
        printf("%s", e.what());
    }catch(...){
        printf("Catched exception\n");
    }
    domobox::DStateContext fsm;
    fsm.Run();
}
