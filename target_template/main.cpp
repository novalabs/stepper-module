#include <ModuleConfiguration.hpp>
#include <Module.hpp>
#include <cstdio>
#include <core/mw/Publisher.hpp>

#define STATUS_TH_SD                   0x0800 // Thermal shutdown
#define STATUS_OCD                     0x1000 // Overcurrent detected

// --- MESSAGES ---------------------------------------------------------------
#include <core/common_msgs/Led.hpp>
#include <core/common_msgs/String64.hpp>

// --- NODES ------------------------------------------------------------------
#include <core/led/Subscriber.hpp>

// --- BOARD IMPL -------------------------------------------------------------
#include <core/L6470_driver/L6470.hpp>

// *** DO NOT MOVE ***
Module module;

// --- TYPES ------------------------------------------------------------------

// --- NODES ------------------------------------------------------------------
core::led::Subscriber led_subscriber("led_subscriber", core::os::Thread::PriorityEnum::LOWEST);


static THD_WORKING_AREA(myThreadWorkingArea, 512);
static THD_FUNCTION(myThread, arg) {
   core::mw::Node node("test_pub");
   core::mw::Publisher<core::common_msgs::String64> pub;
   core::common_msgs::String64* msgp;

   (void)arg;
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
      } else if (!(status & STATUS_OCD)) {
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

      core::os::Thread::sleep(core::os::Time::ms(10));
   }
}

// --- MAIN -------------------------------------------------------------------
extern "C" {
   int
   main()
   {
      module.initialize();

      // Add nodes to the node manager (== board)...
      module.add(led_subscriber);

      // Module configuration

      // Nodes configuration
      core::led::SubscriberConfiguration led_subscriber_configuration;
      led_subscriber_configuration.topic = "led";
      led_subscriber.setConfiguration(led_subscriber_configuration);

      // ... and let's play!
      module.setup();
      module.run();

      (void)chThdCreateStatic(myThreadWorkingArea, sizeof(myThreadWorkingArea), NORMALPRIO, myThread, NULL);

      module.stepper.resetPosition();

      // Is everything going well?
      for (;;) {
         if (!module.isOk()) {
            module.halt("This must not happen!");
         }

         module.keepAlive();

#if 0
         module.stepper.run(1000);
         core::os::Thread::sleep(core::os::Time::ms(500));
#endif

#if 1
         module.stepper.move(200 * 128 / 4);
         core::os::Thread::sleep(core::os::Time::ms(500));

         module.stepper.move(-200 * 128 / 4);
         core::os::Thread::sleep(core::os::Time::ms(500));
#endif

#if 0
         //module.stepper.resetPosition();

         module.stepper.moveto(200 * 128 / 2);
         core::os::Thread::sleep(core::os::Time::ms(2000));

         module.stepper.moveto(-200 * 128 / 2);
         core::os::Thread::sleep(core::os::Time::ms(2000));
#endif

//			Module::led.toggle();
      }

      return core::os::Thread::OK;
   } // main
}
