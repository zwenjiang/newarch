#include "ConveyorReceiveAction.h"
#include "Conveyor.h"

namespace xero {
    namespace droid {
        ConveyorReceiveAction::ConveyorReceiveAction(Conveyor &subsystem):
            ConveyorStateAction(subsystem, "ConveyorReceiveAction") {
            
            const std::string waitForBall = "loop";
            const std::string done = "done";
            setStates({
                { waitForBall, setMotorState(MotorState::Stopped) },
                // if full, stop
                branchState(done, [=] { return getSubsystem().isFull(); } ),

                assertState([=]() { return getSubsystem().isEmpty() 
                                        || getSubsystem().readSensor(Sensor::B);
                            },  "ConveyorReceiveAction called with balls out of position; "
                                "was ConveyorPrepareToEmitAction run?"
                ),
                
                { "wait for collect start sensor", waitForSensorState(Sensor::A, true) },
                
                // we've got a ball
                incrementBallsState(),
                setMotorState(MotorState::MoveTowardsShooter),

                delayState(0.2),
                { "wait for collect finish sensor", waitForSensorState(Sensor::B, true) },

                // ball collected, now collect another
                gotoState(waitForBall),

                { done, setMotorState(MotorState::Stopped) },
            });
        }
    }
}
