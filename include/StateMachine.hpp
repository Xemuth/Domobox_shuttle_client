#ifndef STATE_MACHINE_HEADER
#define STATE_MACHINE_HEADER
#include "Observer.hpp"
#include <memory>
/*
    Abrasct state class dedictated to the Final state machine of Domobox shuttle project
    The state machine is inspired from GoF Design pattern book. 
*/

#define NAME(x) StateName(x)

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
            virtual void Update(const char* subject) = 0;
            virtual std::unique_ptr<DState> SetState(ALL_STATES state) = 0;
            virtual std::unique_ptr<DState> Next() = 0;
    };

    class DStateContext: public Observer{
        public:
            DStateContext();
            ~DStateContext();

            bool SetState(ALL_STATES state);
            void Update(const char* subject); // Observer notifcation entry point
            void Run();

        private:
            std::unique_ptr<DState> state;
    };

    // Сustom definitions of names for enum.
    // Specialization of `enum_name` must be injected in `namespace magic_enum::customize`.
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