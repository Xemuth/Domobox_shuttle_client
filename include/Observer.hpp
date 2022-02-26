#ifndef OBSERVER_HEADER
#define OBSERVER_HEADER

#include <list>

/*
    Implementation of Observer pattern. Inspired from GoF Design pattern book. 
*/

namespace domobox{
       
    class Observer{
        public:
            virtual void Update(const char* subject_name) = 0;

    };

    class Subject{
        public:
            void Attach(Observer& oberser);
            void Detatch(Observer& observer);
            void Notify();
            virtual const char* GetName() = 0; //To Redefine
        private:
            std::list<Observer*> observers;
    };

}

#endif