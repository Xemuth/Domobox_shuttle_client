#ifndef STATE_MACHINE_HEADER
#define STATE_MACHINE_HEADER

#include "Observer.hpp"

/*
    Abrasct state class dedictated to the Final state machine of Domobox shuttle project
    The state machine is inspired from GoF Design pattern book. 
*/

namespace domobox{

    enum ALL_STATES{
        

    };
    
    class DState{
        public:
            virtual void Update(const char* subject) = 0;
            virtual void SetState(ALL_STATES state) = 0;        
    };

    class DStateContext: public Observer{
        public:
            DStateContext();
            ~DStateContext();

            bool SetState(ALL_STATES state);
            void Update(const char* subject); // Observer notifcation entry point
        private:
            DState* state = nullptr;
    };

}

#endif