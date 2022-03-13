/********************************************************           
* StateMachine -- Domobox rely is behavior on a state  *
* machine. This one rely on the GoF state pattern      *   
*                                                      *   
* Author:  Clément Hamon                               *   
********************************************************/  
#include <exception>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "StateMachine.hpp"
#include "States/Initialisation.hpp"

namespace domobox{

    DStateContext::DStateContext(): state(std::unique_ptr<S_Initialisation>(new S_Initialisation)){
        printf("Context state created\n");
    }

    DStateContext::~DStateContext(){
        printf("Context state destroyed\n");
    }
    
    void DStateContext::Run(){ // Attempt to move to next step.
        for(;;){
            auto result = std::move(state->Next());
            if(result){
                auto to_release = state.release();
                delete to_release;
                state = std::move(result);
            }else{
                vTaskDelay(500 / portTICK_RATE_MS);
            }
        }
    }    
}