/*******************************************************
* StateMachine -- Domobox rely is behavior on a state  *
* machine. This one rely on the GoF state pattern      *
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/
#ifndef STATE_MACHINE_HEADER
#define STATE_MACHINE_HEADER
#include <memory>
/*
    Abrasct state class dedictated to the Final state machine of Domobox shuttle project
    The state machine is inspired from GoF Design pattern book. 
*/

namespace domobox{

    enum ALL_STATES{
        INITIALISATION = 0,
        CREDENTIAL_ACQUISITION = 1,
        SERVER_CREATION = 2,
        READY = 3,
        ALARM = 4
    };
    
    class DState{
        public:
            virtual ~DState(){};
            virtual ALL_STATES GetName() const = 0;
            virtual std::unique_ptr<DState> Next() = 0;
    };

    class DStateContext{
        public:
            DStateContext();
            ~DStateContext();
            void Run();

        private:
            std::unique_ptr<DState> state;
    };

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
            case ALL_STATES::ALARM:
                return "ALARM";
        }
        return "UNKNOWN"; 
    }
}

#endif