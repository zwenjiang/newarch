#include "ConveyorPrepareToEmitAction.h"
#include "Conveyor.h"

namespace xero {
    namespace droid {
        void ConveyorPrepareToEmitAction::start() {
            // set motor power to move balls to front
        }

        void ConveyorPrepareToEmitAction::run() {
            // if sensor detects {
                getSubsystem().setMotor(1);
                setDone();
            // }
        }
    }
}