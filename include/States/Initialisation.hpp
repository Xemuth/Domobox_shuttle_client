/*******************************************************
* Initialisation state -- We init all esp32 features   *
* needed in domobox, in case of error move to error    *
* state and print the error description until reset.   *
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/
#ifndef STATE_INITIALISATION_HEADER
#define STATE_INITIALISATION_HEADER
#include "StateMachine.hpp"

namespace domobox{

    class S_Initialisation: public domobox::DState{
        public:
            S_Initialisation();
            virtual ~S_Initialisation();

            ALL_STATES GetName() const;
            void Update(const char* subject);
            std::unique_ptr<DState> SetState(ALL_STATES state);
            std::unique_ptr<DState> Next();
    };

}

#endif