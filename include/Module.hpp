/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <ModuleConfiguration.hpp>
#include <core/mw/CoreModule.hpp>

namespace sensors {}

// Forward declarations
namespace core {
namespace L6470_driver {
class L6470;
}
}

class Module:
    public core::mw::CoreModule
{
public:
// --- DEVICES ----------------------------------------------------------------
    static core::L6470_driver::L6470& stepper;
// ----------------------------------------------------------------------------

    static bool
    initialize();


    Module();
    virtual ~Module() {}
};
