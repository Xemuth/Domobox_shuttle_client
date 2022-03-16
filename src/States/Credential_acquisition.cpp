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
static const int FAILLED_BIT = BIT1;
static const int TRY_CONNECT_BIT = BIT2;
static const int RELEASE_BIT = BIT3;

static TaskHandle_t handler; 
static gpio_num_t led_to_blink;

static volatile EventGroupHandle_t smart_config_management = xEventGroupCreate();
static volatile EventGroupHandle_t wifi_management = xEventGroupCreate();
static volatile EventGroupHandle_t state_machine = xEventGroupCreate();
static volatile SemaphoreHandle_t mutex = xSemaphoreCreateMutex();

static void configure_and_connect(domobox::DomoboxConfiguration& configuration);
static void configure_and_start_smart_configuration(domobox::DomoboxConfiguration& configuration);

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
    configure_and_start_smart_configuration(configuration);
    if(configuration.is_error){
        esp_smartconfig_stop();
        vTaskDelete(NULL);
    }
    while(1){
        uxBits = xEventGroupWaitBits(smart_config_management, CONNECTED_BIT | FAILLED_BIT, true, false, portMAX_DELAY);
        if((uxBits & CONNECTED_BIT) || configuration.is_error) {
            esp_smartconfig_stop();
            xEventGroupSetBits(wifi_management, RELEASE_BIT);
            vTaskDelete(NULL);
        }
        if(uxBits & FAILLED_BIT){
            xEventGroupClearBits(smart_config_management, FAILLED_BIT);
            configure_and_start_smart_configuration(configuration);
        }
    }
}

static void wifi_connection(void * parm){
    domobox::DomoboxConfiguration& configuration = *(static_cast<domobox::DomoboxConfiguration*>(parm));
    if(configuration.has_credential)
        xEventGroupSetBits(wifi_management, TRY_CONNECT_BIT);
    EventBits_t uxBits;
    while(1){
        uxBits = xEventGroupWaitBits(wifi_management, CONNECTED_BIT | TRY_CONNECT_BIT | RELEASE_BIT, true, false, portMAX_DELAY);
        if(uxBits & CONNECTED_BIT && uxBits & RELEASE_BIT){
            configuration.is_connected = true;
            xEventGroupSetBits(state_machine, RELEASE_BIT);
            vTaskDelete(NULL);
        }
        if(uxBits & TRY_CONNECT_BIT){
            configure_and_connect(configuration);
        }
        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    static bool reset_sc = false;
    domobox::DomoboxConfiguration& configuration = *(static_cast<domobox::DomoboxConfiguration*>(arg));
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        configuration.is_connected = false;
        xEventGroupClearBits(smart_config_management, CONNECTED_BIT);
        if(reset_sc) {
            xEventGroupSetBits(smart_config_management, FAILLED_BIT);
            reset_sc = false;
            vTaskResume(handler);
        }
    }else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        xEventGroupSetBits(wifi_management, CONNECTED_BIT);
        xEventGroupSetBits(smart_config_management, CONNECTED_BIT);
    }else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD){
        vTaskSuspend(handler);
        gpio_set_level(led_to_blink, true);
        xEventGroupClearBits(wifi_management, TRY_CONNECT_BIT);
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        {
            MutexLock _(mutex);
            memcpy(configuration.ssid, evt->ssid, sizeof(evt->ssid));
            memcpy(configuration.password, evt->password, sizeof(evt->password));
            configuration.bssid_set = evt->bssid_set;
            configuration.has_credential = true;
        }
        reset_sc = true;
        xEventGroupSetBits(wifi_management, TRY_CONNECT_BIT);
    }
}

static void configure_and_connect(domobox::DomoboxConfiguration& configuration){
    esp_err_t err_rc;
    wifi_config_t wifi_config;
    {
        MutexLock _(mutex);
        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, configuration.ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, configuration.password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = configuration.bssid_set;
        if (wifi_config.sta.bssid_set == true) 
            memcpy(wifi_config.sta.bssid, configuration.bssid, sizeof(wifi_config.sta.bssid));
    }
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

static void configure_and_start_smart_configuration(domobox::DomoboxConfiguration& configuration){
    static bool is_launch = false;
    esp_err_t err_rc;
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    if(is_launch){
        esp_smartconfig_stop();
    }
    if(is_launch || ((err_rc = esp_smartconfig_set_type(SC_TYPE_ESPTOUCH)) == ESP_OK)){
        is_launch = false;
        if ((err_rc = esp_smartconfig_start(&cfg)) == ESP_OK){
            is_launch = true;
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
}

static void blink_task(void * parm){
    bool state = false;
    while(1){
        state = !state;
        gpio_set_level(led_to_blink, state);
        vTaskDelay(400 / portTICK_RATE_MS);
    }
}

namespace domobox{

    S_CredentialAcquisition::S_CredentialAcquisition() : configuration(DomoboxConfiguration::Get()){
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = (1ULL << led);
        gpio_config(&io_conf);
    }

    S_CredentialAcquisition::~S_CredentialAcquisition(){
        gpio_set_level(led, false);
    }

    ALL_STATES S_CredentialAcquisition::GetName() const {return ALL_STATES::CREDENTIAL_ACQUISITION;}
 
    std::unique_ptr<DState> S_CredentialAcquisition::Next(){
        // We create 2 task, Wifi connection and Smart App crendential acquisition:
        // Smart App credential is waiting for smart configuration protocol to be ran off and register credential to DomoboxConfiguration, it swap has_credential boolean to true and kill itself.
        // Wifi connection rely on DomoboxCOnfiguration has_credential boolean to try to connect to wifi. If wifi connection succed then it swap is_connected boolean to true and kill itself.
        DOMOBOX_FSM_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, &configuration));
        DOMOBOX_FSM_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, &configuration));
        DOMOBOX_FSM_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, &configuration));
        
        if(configuration.has_credential) xEventGroupSetBits(wifi_management, TRY_CONNECT_BIT);
        led_to_blink = led;
        xTaskCreate(blink_task, "blink_task",1024, nullptr, 1, &handler);
        xTaskCreate(credential_acquisition, "credential_acquisition_task", 4096, &configuration, 2, nullptr);
        xTaskCreate(wifi_connection, "wifi_connection_task", 4096, &configuration, 2, nullptr);
        xEventGroupWaitBits(state_machine, RELEASE_BIT, true, true, portMAX_DELAY);
        vTaskDelete(handler);
        if(configuration.is_error){
            return std::unique_ptr<S_Error>(new S_Error(std::move(configuration.error))); // We don't care about moving the string, the error state is a final state
        }else if(configuration.is_connected){
            return std::unique_ptr<S_Error>(new S_Error("Is connected to wifi \n")); // Should move to next step
        }
        return {};
    }
}