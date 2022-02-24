#include <stdio.h>
#include "sdkconfig.h"
#include "StateMachine.hpp"

extern "C" {
    void app_main(void);
}


class Test{
    public:
        Test(){
            printf("C++ Working !\n");
        }
        ~Test(){
            printf("Object deletion\n");
        }
};

void app_main()
{
    Test test;
    domobox::DState state;

}
