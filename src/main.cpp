#include <stdio.h>
#include "sdkconfig.h"

extern "C" {
    void app_main(void);
}


class Test{
    public:
        Test(){
            printf("C++ Working !");
        }
        ~Test(){
            printf("Object deletion");
        }
};

void app_main()
{
    Test test;

}
