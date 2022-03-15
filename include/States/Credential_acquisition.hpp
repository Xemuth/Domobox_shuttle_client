/*********************************************************
* Credential acquisition state -- Waiting for credential *
* using smart configuration protocol.                    *
*                                                        *
* Author:  Clément Hamon                                 *
**********************************************************/
#ifndef STATE_CREDENTIAL_ACQUISITION_HEADER
#define STATE_CREDENTIAL_ACQUISITION_HEADER

#include "StateMachine.hpp"
#include "Definition.hpp"

namespace domobox{

    class S_CredentialAcquisition: public domobox::DState{
        public:
            S_CredentialAcquisition();
            virtual ~S_CredentialAcquisition();

            ALL_STATES GetName() const;
            std::unique_ptr<DState> Next();


        private:
            DomoboxConfiguration& configuration;
            bool task_created;
    };

}

#endif