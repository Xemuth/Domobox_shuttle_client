/*********************************************************
* Credential acquisition state -- Waiting for credential *
* using smart configuration protocol.                    *
*                                                        *
* Author:  Clément Hamon                                 *
**********************************************************/
#include "States/Credential_acquisition.hpp"

namespace domobox{
    S_CredentialAcquisition::S_CredentialAcquisition(){}

    S_CredentialAcquisition::~S_CredentialAcquisition(){}

    ALL_STATES S_CredentialAcquisition::GetName() const {return ALL_STATES::CREDENTIAL_ACQUISITION;}

    std::unique_ptr<DState> S_CredentialAcquisition::Next(){

        return std::unique_ptr<S_Error>(new S_Error("To complete\n"));
    }
}