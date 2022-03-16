/*******************************************************
* Server creation state -- We create the udp server    *
* and we request for each ip present on the network    *
* until we found the master server.                    *
*                                                      *
* Author:  Clément Hamon                               *
********************************************************/

#include <driver/gpio.h>
#include "StateMachine.hpp"
#include "Definition.hpp"

namespace domobox{

    class S_ServerCreation: public domobox::DState{
        public:
            S_ServerCreation();
            virtual ~S_ServerCreation();

            ALL_STATES GetName() const;
            std::unique_ptr<DState> Next();

        private:
            DomoboxConfiguration& configuration;
            gpio_config_t io_conf;
            gpio_num_t led = GPIO_NUM_18;
    };

}

#endif