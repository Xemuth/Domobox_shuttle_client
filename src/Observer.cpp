#include "Observer.hpp"


namespace domobox {
    void Subject::Attach(Observer& observer){
        observers.insert(observers.cend(), &observer);
    }

    void Subject::Detatch(Observer& observer){
        for(std::__cxx11::list<domobox::Observer *>::iterator iter = observers.begin(); iter != observers.end(); iter++){ // madness iterator
            if(&observer == *iter){
                observers.erase(iter);
                return;
            }
        }
        return;
    }

    void Subject::Notify(){
        for(Observer* observer: observers)
            observer->Update(GetName());
    }

}