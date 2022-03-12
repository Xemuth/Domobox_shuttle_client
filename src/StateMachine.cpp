#include "StateMachine.hpp"
#include "States/Initialisation.hpp"
#include <stdio.h>
#include <exception>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
namespace domobox{

    DStateContext::DStateContext(): state(std::unique_ptr<S_Initialisation>(new S_Initialisation)){
        printf("Context state created\n");
    }

    DStateContext::~DStateContext(){
        printf("Context state destroyed\n");
    }
    
    bool DStateContext::SetState(ALL_STATES state_to_swap){
        auto result = std::move(state->SetState(state_to_swap));
        bool updated = false;
        if(result){
            updated = true;
            auto to_release = state.release();
            delete to_release;
            state = std::move(result);
        }
        printf("%s\n", NAME(state->GetName()));
        return updated; // True if state has changed !
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

    void DStateContext::Update(const char* subject){
        printf("Notified from %s\n", subject);
        if(state)
            state->Update(subject);
    }
    
}