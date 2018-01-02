/* COPYRIGHT (c) 2016-2018 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/snippets/CortexMxFaultHandlers.h>

#include <core/mw/Middleware.hpp>
#include <core/mw/transport/RTCANTransport.hpp>

#include <core/hw/EXT.hpp>
#include <core/hw/GPIO.hpp>
#include <core/hw/SPI.hpp>
#include <core/hw/IWDG.hpp>
#include <core/os/Thread.hpp>

#include <Module.hpp>

#include <core/L6470_driver/L6470.hpp>

// LED
using LED_PAD = core::hw::Pad_<core::hw::GPIO_B, LED_PIN>;
static LED_PAD _led;

// DEVICES
static core::hw::EXTController_<core::hw::EXT_1>    _ext;
static core::hw::Pad_<core::hw::GPIO_B, GPIOB_FLAG> _driver_flag;
static core::hw::Pad_<core::hw::GPIO_B, GPIOB_STBY> _driver_stby;
static core::hw::EXTChannel_<core::hw::EXT_1, GPIOB_BUSY, EXT_CH_MODE_RISING_EDGE | EXT_MODE_GPIOB> _driver_busy;
static core::hw::SPIDevice_<core::hw::SPI_2, core::hw::Pad_<core::hw::GPIO_B, GPIOB_SPI_CS> >       _driver_spi;

// DRIVER
static core::L6470_driver::L6470 _l6470(_driver_spi, _driver_busy, _driver_flag, _driver_stby);

// MODULE DEVICES
core::L6470_driver::L6470& Module::stepper = _l6470;

// DEVICE CONFIG
static const SPIConfig _spi_config = {
    0, 0, 0, SPI_CR1_BR_1 | SPI_CR1_CPOL | SPI_CR1_CPHA, 0
};

static EXTConfig _ext_config = {    {
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


// SYSTEM STUFF
static core::os::Thread::Stack<1024> management_thread_stack;
static core::mw::RTCANTransport      rtcantra(&RTCAND1);

RTCANConfig rtcan_config = {
    1000000, 100, 60
};


Module::Module()
{}

bool
Module::initialize()
{
#ifdef _DEBUG
    FAULT_HANDLERS_ENABLE(true);
#else
    FAULT_HANDLERS_ENABLE(false);
#endif

    static bool initialized = false;

    if (!initialized) {
        core::mw::CoreModule::initialize();

        core::mw::Middleware::instance().initialize(name(), management_thread_stack, management_thread_stack.size(), core::os::Thread::LOWEST);
        rtcantra.initialize(rtcan_config, canID());
        core::mw::Middleware::instance().start();

        _ext.start(_ext_config);
        _driver_spi.start(_spi_config);

        stepper.probe();

        initialized = true;
    }

    return initialized;
} // Board::initialize

// ----------------------------------------------------------------------------
// CoreModule STM32FlashConfigurationStorage
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleSTM32FlashConfigurationStorage.hpp>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// CoreModule HW specific implementation
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleHWSpecificImplementation.hpp>
// ----------------------------------------------------------------------------
