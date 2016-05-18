/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */
 
#pragma once

#include <Configuration.hpp>
#include <Core/MW/CoreModule.hpp>
#include <Core/MW/CoreSensor.hpp>

namespace sensors {}

// Forward declarations
namespace drivers {
   class L6470;
}

class Module:
   public Core::MW::CoreModule
{
public:
// --- DEVICES ----------------------------------------------------------------
   static drivers::L6470& stepper;
// ----------------------------------------------------------------------------

   static bool
   initialize();


   Module();
   virtual ~Module() {}
};
