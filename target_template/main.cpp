#include <Configuration.hpp>
#include <Module.hpp>
#include <cstdio>
#include <Core/MW/Publisher.hpp>

#define STATUS_TH_SD                   0x0800 // Thermal shutdown
#define STATUS_OCD                     0x1000 // Overcurrent detected

// --- MESSAGES ---------------------------------------------------------------
#include <common_msgs/Led.hpp>
#include <common_msgs/String64.hpp>

// --- NODES ------------------------------------------------------------------
#include <led/Subscriber.hpp>

// --- BOARD IMPL -------------------------------------------------------------
#include <L6470_driver/L6470.hpp>

// *** DO NOT MOVE ***
Module module;

// --- TYPES ------------------------------------------------------------------

// --- NODES ------------------------------------------------------------------
led::Subscriber led_subscriber("led_subscriber", Core::MW::Thread::PriorityEnum::LOWEST);


static THD_WORKING_AREA(myThreadWorkingArea, 512);
static THD_FUNCTION(myThread, arg) {
	Core::MW::Node node("test_pub");
	Core::MW::Publisher<common_msgs::String64> pub;
	common_msgs::String64 * msgp;

	(void) arg;
	chRegSetThreadName("test_pub");

	node.advertise(pub, "test");
	chThdSleepMilliseconds(500);

	while (true) {
		uint16_t status = module.stepper.getStatus();

		if (!(status & STATUS_TH_SD)) {
			if (pub.alloc(msgp)) {
				sprintf(msgp->data._data, "\nTH_SD\n");
				pub.publish(msgp);
			}
		} else if (!(status & STATUS_OCD)){
			if (pub.alloc(msgp)) {
				sprintf(msgp->data._data, "\nOCD\n");
				pub.publish(msgp);
			}
		} else {
			if (pub.alloc(msgp)) {
				sprintf(msgp->data._data, ".");
				pub.publish(msgp);
			}
		}

		Core::MW::Thread::sleep(Core::MW::Time::ms(10));
	}
}

// --- MAIN -------------------------------------------------------------------
extern "C" {
	int	main(){
		module.initialize();

		// Module configuration

		// Nodes configuration
		led_subscriber.configuration["topic"] = "led";

		// Add nodes to the node manager (== board)...
		module.add(led_subscriber);

		// ... and let's play!
		module.setup();
		module.run();

		(void)chThdCreateStatic(myThreadWorkingArea, sizeof(myThreadWorkingArea),
		                          NORMALPRIO, myThread, NULL);

		module.stepper.resetPosition();

		// Is everything going well?
		for (;;) {
			if (!module.isOk()) {
				module.halt("This must not happen!");
			}

#if 0
			module.stepper.run(1000);
			Core::MW::Thread::sleep(Core::MW::Time::ms(500));
#endif

#if 1
			module.stepper.move(200*128/4);
			Core::MW::Thread::sleep(Core::MW::Time::ms(500));

			module.stepper.move(-200*128/4);
			Core::MW::Thread::sleep(Core::MW::Time::ms(500));
#endif

#if 0
			//module.stepper.resetPosition();

			module.stepper.moveto(200*128/2);
			Core::MW::Thread::sleep(Core::MW::Time::ms(2000));

			module.stepper.moveto(-200*128/2);
			Core::MW::Thread::sleep(Core::MW::Time::ms(2000));
#endif

//			Module::led.toggle();
		}

		return Core::MW::Thread::OK;
	} // main
}
