#ifndef STATE_MACHINE_HEADER
#define STATE_MACHINE_HEADER

#include "Observer.hpp"
#include <memory>
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
            virtual std::unique_ptr<DState> SetState(ALL_STATES state, std::unique_ptr<DState>&& current) = 0;
            virtual void Run() = 0;
    };

    class NullState: public domobox::DState{
        public:
            void Update(const char* subject);
            std::unique_ptr<DState> SetState(ALL_STATES state, std::unique_ptr<DState>&& current);
            void Run();
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

}

#endif