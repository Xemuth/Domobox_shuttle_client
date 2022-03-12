
#include "StateMachine.hpp"
#include "States/Initialisation.hpp"
#include "States/Error.hpp"

namespace domobox{

    S_Initialisation::S_Initialisation(){
        printf("Constructor\n");
    }

    S_Initialisation::~S_Initialisation(){
        printf("Destructor\n");
    }
    
    ALL_STATES S_Initialisation::GetName() const{return ALL_STATES::INITIALISATION;}

    void S_Initialisation::Update(const char* subject){
        printf("%s\n", __PRETTY_FUNCTION__);
    }

    std::unique_ptr<DState> S_Initialisation::SetState(ALL_STATES state){
        printf("%s\n", __PRETTY_FUNCTION__);
        return {};
    }

    std::unique_ptr<DState> S_Initialisation::Next(){
        //For now we are only going to Error state
        return std::unique_ptr<S_Error>(new S_Error);
    }
}