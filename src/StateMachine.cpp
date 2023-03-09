/*******************************************************
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

    static const char* StateName(ALL_STATES states){
        switch (states) {
            case ALL_STATES::INITIALISATION:
                return "INITIALISATION";
            case ALL_STATES::CREDENTIAL_ACQUISITION:
                return "CREDENTIAL_ACQUISITION";
            case ALL_STATES::SERVER_CREATION:
                return "SERVER_CREATION";
            case ALL_STATES::READY:
                return "READY";
            case ALL_STATES::ERROR:
                return "ERROR";
        }
        return "UNKNOWN"; 
    }

    DStateContext::DStateContext(): state(std::unique_ptr<S_Initialisation>(new S_Initialisation)){}
    DStateContext::~DStateContext(){}
    
    void DStateContext::Run(){ // Attempt to move to next step.
        for(;;){
            std::unique_ptr<DState> result = std::move(state->Next());
            if(result){
                printf("Domobox_shuttle: Moving from %s to %s\n", StateName(state->GetName()), StateName(result->GetName()));
                auto to_release = state.release();
                delete to_release;
                state = std::move(result);
            }else{
                vTaskDelay(2000 / portTICK_PERIOD_MS);
            }
        }
    }    
}