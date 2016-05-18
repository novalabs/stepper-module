/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#include <Core/MW/Middleware.hpp>

#include "ch.h"
#include "hal.h"

#include <Core/HW/EXT.hpp>
#include <Core/HW/GPIO.hpp>
#include <Core/HW/SPI.hpp>
#include <Core/MW/Thread.hpp>
#include <Module.hpp>

#include <L6470_driver/L6470.hpp>

/* LED */
using LED_PAD = Core::HW::Pad_<Core::HW::GPIO_B, GPIOB_LED>;
static LED_PAD _led;

/* SPI */
using SPI_CS_PAD = Core::HW::Pad_<Core::HW::GPIO_B, GPIOB_SPI_CS>;
static Core::HW::SPIDevice_<Core::HW::SPI_2, SPI_CS_PAD> SPI_DEVICE;

static const SPIConfig spicfg = {
   0, 0, 0, SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA, 0
};

/* EXT */
static Core::HW::EXTChannel_<Core::HW::EXT_1, GPIOB_BUSY, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOB> BUSY_EXT_CHANNEL;

static EXTConfig extcfg = {   {
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL},
                                 {EXT_CH_MODE_DISABLED, NULL}
                              }};


/* GPIO */
static Core::HW::Pad_<Core::HW::GPIO_B, GPIOB_FLAG> _flag_pad;
static Core::HW::Pad_<Core::HW::GPIO_B, GPIOB_STBY> _stby_pad;

static drivers::L6470 _stepper(SPI_DEVICE, BUSY_EXT_CHANNEL, _flag_pad, _stby_pad);

drivers::L6470& Module::stepper = _stepper;

/* MW and RTCAN */
static THD_WORKING_AREA(wa_info, 1024);
static Core::MW::RTCANTransport rtcantra(RTCAND1);

RTCANConfig rtcan_config = {
   1000000, 100, 60
};

#ifndef CORE_MODULE_NAME
#define CORE_MODULE_NAME "Stepper"
#endif

Core::MW::Middleware Core::MW::Middleware::instance(CORE_MODULE_NAME, "BOOT_" CORE_MODULE_NAME);


Module::Module()
{}

bool
Module::initialize()
{
//	CORE_ASSERT(Core::MW::Middleware::instance.is_stopped()); // TODO: capire perche non va...

   static bool initialized = false;


   if (!initialized) {
      halInit();

      chSysInit();

      Core::MW::Middleware::instance.initialize(wa_info, sizeof(wa_info), Core::MW::Thread::LOWEST);
      rtcantra.initialize(rtcan_config);
      Core::MW::Middleware::instance.start();

      spiStart(&SPID2, &spicfg);
      extStart(&EXTD1, &extcfg);

      stepper.probe();

      initialized = true;
   }

   return initialized;
} // Board::initialize

// Leftover from CoreBoard (where LED_PAD cannot be defined
void
Core::MW::CoreModule::Led::toggle()
{
   _led.toggle();
}

void
Core::MW::CoreModule::Led::write(
   unsigned on
)
{
   _led.write(on);
}
