#include "StateMachine.hpp"
#include <stdio.h>

namespace domobox{

    DState::DState(){
        printf("State created\n");
    }

    DState::~DState(){
        printf("State destroyed\n");
    }

}