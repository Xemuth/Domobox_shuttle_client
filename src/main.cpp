/********************************************************           
* Main -- Entry point of dombox project                *
*                                                      *   
* Author:  Clément Hamon                               *   
********************************************************/  
#include "StateMachine.hpp"
#include "Definition.hpp"

using namespace domobox;

extern "C" {
    void app_main(void);
}

void app_main() 
{
    DStateContext fsm;
    fsm.Run();
}
