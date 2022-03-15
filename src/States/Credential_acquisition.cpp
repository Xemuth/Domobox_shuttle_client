/*********************************************************
* Credential acquisition state -- Waiting for credential *
* using smart configuration protocol.                    *
* Try to connect to wifi.                                 *
*                                                        *
* Author:  Clément Hamon                                 *
**********************************************************/
#include "States/Credential_acquisition.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static volatile SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

// Simple janitor
struct MutexLock{
    MutexLock(SemaphoreHandle_t mutex) : handler(mutex) {xSemaphoreTake(handler, portMAX_DELAY);}
    ~MutexLock(){xSemaphoreGive(handler);}
    private:
        SemaphoreHandle_t handler;
};

// Smart App credential is waiting for smart configuration protocol to be ran off and register credential 
// to DomoboxConfiguration, it swap has_credential boolean to true and kill itself.
static void credential_acquisition(void * parm){
    static int cpt = 0;
    domobox::DomoboxConfiguration& configuration = *(static_cast<domobox::DomoboxConfiguration*>(parm));
    while(1){
        if(configuration.is_error) vTaskDelete(nullptr);
        if(cpt > 30) {
            {
                MutexLock _(mutex);
                configuration.error = "Failled to connect to SMART APP, unknown error\n";
                configuration.is_error = true;
            }
            vTaskDelete(nullptr);
        }
        cpt++;
    }
}

// Wifi connection rely on DomoboxCOnfiguration has_credential boolean to try to connect to wifi.
// If wifi connection succed then it swap is_connected boolean to true and kill itself.
static void wifi_connection(void * parm){
    static int cpt = 0;
    domobox::DomoboxConfiguration& configuration = *(static_cast<domobox::DomoboxConfiguration*>(parm));
    while(1){
        if(configuration.is_error) vTaskDelete(nullptr);
        if(cpt > 30){
            {
                MutexLock _(mutex);
                configuration.error = "Failled to connect to wifi, unknown error\n";
                configuration.is_error = true;
            }
            vTaskDelete(nullptr);
        }
        cpt++;
    }
}

namespace domobox{
    S_CredentialAcquisition::S_CredentialAcquisition() : configuration(DomoboxConfiguration::Get()), task_created(false) {}

    S_CredentialAcquisition::~S_CredentialAcquisition(){}

    ALL_STATES S_CredentialAcquisition::GetName() const {return ALL_STATES::CREDENTIAL_ACQUISITION;}
 
    std::unique_ptr<DState> S_CredentialAcquisition::Next(){
        // We create 2 task, Wifi connection and Smart App crendential acquisition:
        // Smart App credential is waiting for smart configuration protocol to be ran off and register credential to DomoboxConfiguration, it swap has_credential boolean to true and kill itself.
        // Wifi connection rely on DomoboxCOnfiguration has_credential boolean to try to connect to wifi. If wifi connection succed then it swap is_connected boolean to true and kill itself.
        if(!task_created){
            xTaskCreate(credential_acquisition, "credential_acquisition_task", 4096, &configuration, 3, nullptr);
            xTaskCreate(wifi_connection, "wifi_connection_task", 4096, &configuration, 3, nullptr);
        }
        //Since vTaskGetInfo is not available in esp32 package we only rely on configuration object.
        if(configuration.is_error){
            MutexLock _(mutex);
            return std::unique_ptr<S_Error>(new S_Error(std::move(configuration.error))); // We don't care about moving the string, the error state is a final state
        }else if(configuration.is_connected){
            return std::unique_ptr<S_Error>(new S_Error("Is connected to wifi \n")); // Should move to next step
        }
        return {};
    }
}