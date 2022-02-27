#include "StateMachine.hpp"
#include <stdio.h>


namespace domobox{

    void NullState::Update(const char* subject){
        printf("NullState::Update\n");
    }
    std::unique_ptr<DState> NullState::SetState(ALL_STATES state, std::unique_ptr<DState>&& current){
        return std::move(current);
    }
    void NullState::Run(){
        printf("NullState::Run\n");
    }

    DStateContext::DStateContext(): state(std::unique_ptr<NullState>(new NullState())){
        printf("Context state created\n");
    }

    DStateContext::~DStateContext(){
        printf("Context state destroyed\n");
    }
    
    bool DStateContext::SetState(ALL_STATES state_to_swap){
        state = std::move(state->SetState(state_to_swap, std::move(state)));
        return false;
    }

    void DStateContext::Update(const char* subject){
        printf("Notified from ");
        printf(subject);
        printf("\n");
        if(state)
            state->Update(subject);
    }

    void DStateContext::Run(){
        state->Run();
    }

}