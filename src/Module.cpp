/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/mw/Middleware.hpp>
#include <core/mw/transport/RTCANTransport.hpp>

#include "ch.h"
#include "hal.h"

#include <core/hw/EXT.hpp>
#include <core/hw/GPIO.hpp>
#include <core/hw/SPI.hpp>
#include <core/hw/IWDG.hpp>
#include <core/os/Thread.hpp>
#include <Module.hpp>

#include <core/L6470_driver/L6470.hpp>

/* LED */
using LED_PAD = core::hw::Pad_<core::hw::GPIO_B, GPIOB_LED>;
static LED_PAD _led;

/* SPI */
using SPI_CS_PAD = core::hw::Pad_<core::hw::GPIO_B, GPIOB_SPI_CS>;
static core::hw::SPIDevice_<core::hw::SPI_2, SPI_CS_PAD> SPI_DEVICE;

static const SPIConfig spicfg = {
    0, 0, 0, SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA, 0
};

/* EXT */
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOB_BUSY, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOB> BUSY_EXT_CHANNEL;

static EXTConfig extcfg = {    {
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
static core::hw::Pad_<core::hw::GPIO_B, GPIOB_FLAG> _flag_pad;
static core::hw::Pad_<core::hw::GPIO_B, GPIOB_STBY> _stby_pad;

static core::L6470_driver::L6470 _stepper(SPI_DEVICE, BUSY_EXT_CHANNEL, _flag_pad, _stby_pad);

core::L6470_driver::L6470& Module::stepper = _stepper;

/* MW and RTCAN */
static core::os::Thread::Stack<1024> management_thread_stack;
static core::mw::RTCANTransport      rtcantra(&RTCAND1);

RTCANConfig rtcan_config = {
    1000000, 100, 60
};

// ----------------------------------------------------------------------------
// CoreModule STM32FlashConfigurationStorage
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleSTM32FlashConfigurationStorage.hpp>
// ----------------------------------------------------------------------------

core::mw::Middleware
core::mw::Middleware::instance(
    ModuleConfiguration::MODULE_NAME
);


Module::Module()
{}

bool
Module::initialize()
{
    static bool initialized = false;


    if (!initialized) {
        halInit();

        chSysInit();

        core::mw::Middleware::instance.initialize(name(), management_thread_stack, management_thread_stack.size(), core::os::Thread::LOWEST);
        rtcantra.initialize(rtcan_config, canID());
        core::mw::Middleware::instance.start();

        spiStart(&SPID2, &spicfg);
        extStart(&EXTD1, &extcfg);

        stepper.probe();

        initialized = true;
    }

    return initialized;
} // Board::initialize

// ----------------------------------------------------------------------------
// CoreModule HW specific implementation
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleHWSpecificImplementation.hpp>
// ----------------------------------------------------------------------------
