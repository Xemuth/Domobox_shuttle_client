#include <stdio.h>
#include "sdkconfig.h"
#include "StateMachine.hpp"
#include "Observer.hpp"

extern "C" {
    void app_main(void);
}

class test_subject: public domobox::Subject{
    public:
        const char* GetName(){
            return "test_subject";
        }
};

void app_main() 
{
    domobox::DStateContext fsm;
    test_subject subject;
    subject.Attach(fsm);
    fsm.Run();
}
