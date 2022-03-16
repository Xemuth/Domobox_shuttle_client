/*********************************************************
* Credential acquisition state -- Waiting for credential *
* using smart configuration protocol.                    *
* Try to connect to wifi.                                 *
*                                                        *
* Author:  Clément Hamon                                 *
**********************************************************/
#include <string.h>
#include "States/Credential_acquisition.hpp"
#include "esp_event_base.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_smartconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const int CONNECTION_FAILLED = BIT2;
static volatile SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
static volatile EventGroupHandle_t wifi_event_group = xEventGroupCreate();
static void configure_and_connect(domobox::DomoboxConfiguration& configuration);
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
    domobox::DomoboxConfiguration& configuration = *(static_cast<domobox::DomoboxConfiguration*>(parm));
    EventBits_t uxBits;
    esp_err_t err_rc;
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    printf("//////////////////// LAUNCHING CREDENTIAL ACQUISITION\n");
    if ((err_rc = esp_smartconfig_set_type(SC_TYPE_ESPTOUCH)) == ESP_OK){
        if ((err_rc = esp_smartconfig_start(&cfg)) == ESP_OK){
            if(configuration.has_credential)
                configure_and_connect(configuration);
        }else{
            MutexLock _(mutex);
            configuration.error = std::move(domobox::_error_message_preparation(err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, "esp_smartconfig_start(&cfg)"));
            configuration.is_error = true;
        }
    }else{
        MutexLock _(mutex);
        configuration.error = std::move(domobox::_error_message_preparation(err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, "esp_smartconfig_set_type(SC_TYPE_ESPTOUCH)"));
        configuration.is_error = true;
    }
    if(configuration.is_error){
        esp_smartconfig_stop();
        vTaskDelete(NULL);
    }
    while(1){
        uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT | CONNECTION_FAILLED, true, false, portMAX_DELAY);
        if(uxBits & CONNECTION_FAILLED){
            printf("//////////////////// Smart config failled, we reset\n");
            esp_smartconfig_stop();
            
            if((err_rc = esp_smartconfig_start(&cfg)) != ESP_OK){
                MutexLock _(mutex);
                configuration.error = std::move(domobox::_error_message_preparation(err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, "esp_smartconfig_start(&cfg)"));
                configuration.is_error = true;
            }
        }
        if(((uxBits & ESPTOUCH_DONE_BIT) && (uxBits & CONNECTED_BIT)) || configuration.is_error) {
            printf("//////////////////// ESPTOUCH_DONE_BIT or configuration.is_error\n");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    domobox::DomoboxConfiguration& configuration = *(static_cast<domobox::DomoboxConfiguration*>(arg));
    esp_err_t err_rc;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        printf("//////////////////// WIFI_EVENT_STA_DISCONNECTED\n");
        {
            MutexLock _(mutex);
            configuration.is_connected = false;
        }
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        xEventGroupSetBits(wifi_event_group, CONNECTION_FAILLED);
    }else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        printf("//////////////////// IP_EVENT_STA_GOT_IP\n");
        {
            MutexLock _(mutex);
            configuration.is_connected = true;
        }
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE){
        printf("//////////////////// SC_EVENT_SEND_ACK_DONE\n");
        xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
    }else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD){
        printf("//////////////////// SC_EVENT_GOT_SSID_PSWD\n");
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        {
            MutexLock _(mutex);
            memcpy(configuration.ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(configuration.password, evt->password, sizeof(evt->password));
            configuration.bssid_set = evt->bssid_set;
            configuration.has_credential = true;
        }
        configure_and_connect(configuration);       
    }
}

static void configure_and_connect(domobox::DomoboxConfiguration& configuration){
    esp_err_t err_rc;
    wifi_config_t wifi_config;
    bzero(&wifi_config, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, configuration.ssid, sizeof(wifi_config.sta.ssid));
    memcpy(wifi_config.sta.password, configuration.password, sizeof(wifi_config.sta.password));
    wifi_config.sta.bssid_set = configuration.bssid_set;
    if (wifi_config.sta.bssid_set == true) 
        memcpy(wifi_config.sta.bssid, configuration.bssid, sizeof(wifi_config.sta.bssid));

    if((err_rc = esp_wifi_disconnect()) == ESP_OK){
        if((err_rc = esp_wifi_set_config(WIFI_IF_STA, &wifi_config)) == ESP_OK){
            esp_wifi_connect();
        }else{
            MutexLock _(mutex);
            configuration.error = std::move(domobox::_error_message_preparation(err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, "esp_wifi_set_config(WIFI_IF_STA, &wifi_config)"));
            configuration.is_error = true;
        }
    }else{
        MutexLock _(mutex);
        configuration.error = std::move(domobox::_error_message_preparation(err_rc, __FILE__, __LINE__, __PRETTY_FUNCTION__, "esp_wifi_disconnect()"));
        configuration.is_error = true;
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
            task_created = true;
            DOMOBOX_FSM_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, &configuration) );
            DOMOBOX_FSM_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, &configuration) );
            DOMOBOX_FSM_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, &configuration) );
            xTaskCreate(credential_acquisition, "credential_acquisition_task", 4096, &configuration, 3, nullptr);
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