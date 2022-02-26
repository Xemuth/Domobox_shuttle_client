#include "StateMachine.hpp"
#include <stdio.h>

namespace domobox{

    DStateContext::DStateContext(){
        printf("Context state created\n");
    }

    DStateContext::~DStateContext(){
        printf("Context state destroyed\n");
    }
    
    bool DStateContext::SetState(ALL_STATES state_to_swap){
        state->SetState(state_to_swap);
        return false;
    }

    void DStateContext::Update(const char* subject){
        printf("Notified from ");
        printf(subject);
        printf("\n");
        if(state)
            state->Update(subject);
    }

}