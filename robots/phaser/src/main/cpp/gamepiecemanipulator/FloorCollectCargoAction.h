#pragma once

#include "GamePieceAction.h"
#include "GamePieceManipulator.h"

namespace xero{
    namespace phaser{
        class FloorCollectCargoAction : public GamePieceAction{
        public:
            FloorCollectCargoAction(GamePieceManipulator &subsystem) ;
            virtual ~FloorCollectCargoAction() ;

            virtual void start() ;
            virtual void run() ;
            virtual bool isDone() ;
            virtual void cancel() ;
            virtual std::string toString() ;

        private:
            enum class State {
                Idle,
                LifterGoToSafeHeight,
                TurntableGoToCollectAngle,
                LifterGoToCollectHeightDeployIntake,
                WaitForCargo,
                StopAllMotors,
                RaiseLifter,
                RetractIntake
            } ;

        private:
            State state_ ;

            xero::base::ActionPtr set_lifter_safe_height_ ; 
            xero::base::ActionPtr set_turntable_cargo_angle_ ;
            xero::base::ActionPtr set_lifter_cargo_intake_height_ ;
            xero::base::ActionPtr set_lifter_cargo_collected_height_ ;

            xero::base::ActionPtr deploy_cargo_intake_ ;
            xero::base::ActionPtr retract_cargo_intake_ ;

            xero::base::ActionPtr set_cargo_intake_motor_ ;
            xero::base::ActionPtr stop_cargo_intake_motor_ ;
            xero::base::ActionPtr set_cargo_holder_motor_ ;
            xero::base::ActionPtr stop_cargo_holder_motor_ ;
            
        };
    }
}