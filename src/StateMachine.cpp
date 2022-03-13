/********************************************************           
* StateMachine -- Domobox rely is behavior on a state  *
* machine. This one rely on the GoF state pattern      *   
*                                                      *   
* Author:  Clément Hamon                               *   
********************************************************/  
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "StateMachine.hpp"
#include "States/Initialisation.hpp"

namespace domobox{

    DStateContext::DStateContext(): state(std::unique_ptr<S_Initialisation>(new S_Initialisation)){}
    DStateContext::~DStateContext(){}
    
    void DStateContext::Run(){ // Attempt to move to next step.
        for(;;){
            std::unique_ptr<DState> result = std::move(state->Next());
            if(result){
                auto to_release = state.release();
                delete to_release;
                state = std::move(result);
            }else{
                vTaskDelay(5000 / portTICK_RATE_MS);
            }
        }
    }    
}