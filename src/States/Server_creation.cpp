/*******************************************************
* Server creation state -- We create the udp server    *
* and we request for each ip present on the network    *
* until we found the master server.                    *
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/
#include "States/Server_creation.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"