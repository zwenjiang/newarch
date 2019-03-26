#include "ResetIntakesAction.h"
#include <singlemotorsubsystem/SingleMotorPowerAction.h>
#include <lifter/LifterGoToHeightAction.h>
#include <turntable/TurntableGoToAngleAction.h>
#include <cargointake/CargoIntake.h>
#include <cargointake/CargoIntakeAction.h>
#include <carloshatch/CarlosHatchArmAction.h>
#include <cmath>

using namespace xero::base ;

namespace xero {
    namespace phaser {
        ResetIntakeAction::ResetIntakeAction(GamePieceManipulator &subsystem):GamePieceAction(subsystem) {

            auto lifter = getGamePiece().getLifter() ; 
            auto turntable = getGamePiece().getTurntable() ;
            auto cargo_intake = getGamePiece().getCargoIntake() ;
            auto hatch_holder = getGamePiece().getHatchHolder() ;
            auto cargo_holder = getGamePiece().getCargoHolder() ;

            set_lifter_safe_height_ = std::make_shared<LifterGoToHeightAction>(*lifter, "turntable:safe_lifter_height") ;
            set_lifter_level_one_  = std::make_shared<LifterGoToHeightAction>(*lifter, "lifter:height:bottom") ;
            set_turntable_zero_ = std::make_shared<TurntableGoToAngleAction>(*turntable, 0.0) ;
            retract_cargo_intake_ = std::make_shared<CargoIntakeAction>(*cargo_intake, false) ;
            retract_arm_ = std::make_shared<CarlosHatchArmAction>(*hatch_holder, CarlosHatchArmAction::Operation::RETRACT) ;

            stop_cargo_intake_motor_ = std::make_shared<SingleMotorPowerAction>(*cargo_intake, 0.0) ;
            stop_cargo_holder_motor_ = std::make_shared<SingleMotorPowerAction>(*cargo_holder, 0.0) ;

            state_ = State::Idle ;           
        }

        ResetIntakeAction::~ResetIntakeAction() {
        }

        void ResetIntakeAction::start() {
            auto lifter = getGamePiece().getLifter() ;
            auto turntable = getGamePiece().getTurntable() ;
            auto cargo_intake = getGamePiece().getCargoIntake() ;
            auto cargo_holder = getGamePiece().getCargoHolder() ;
            auto hatch_holder = getGamePiece().getHatchHolder() ;

            hatch_holder->setAction(retract_arm_) ;            

            cargo_intake->setAction(stop_cargo_intake_motor_) ;
            cargo_holder->setAction(stop_cargo_holder_motor_) ;

            if (cargo_intake->isDeployed() || std::fabs(turntable->getAngleValue()) > epsilon) {
                lifter->setAction(set_lifter_safe_height_) ;
                state_ = State::SafeHeight ;
            }
            else if (hatch_holder->isArmDeployed()) {
                hatch_holder->setAction(retract_arm_) ;
                state_ = State::RetractArm ;

            }
            else {
                lifter->setAction(set_lifter_level_one_) ;
                state_ = State::FinalHeight ;
            }
        }

        void ResetIntakeAction::run() {
            switch(state_) {
            case State::SafeHeight:
                if (set_lifter_safe_height_->isDone()) {
                    auto turntable = getGamePiece().getTurntable() ;
                    turntable->setAction(set_turntable_zero_) ;
                    state_ = State::RotateZero ;
                }
                break ;
            case State::RotateZero:
                if (set_turntable_zero_->isDone()) {
                    auto cargo_intake = getGamePiece().getCargoIntake();  
                    cargo_intake->setAction(retract_cargo_intake_) ;

                    state_ = State::RetractIntakes ;
                }
                break ;

            case State::RetractIntakes:
                if (retract_cargo_intake_->isDone()) {
                    auto hatch_holder = getGamePiece().getHatchHolder() ;
                    if (hatch_holder->isArmDeployed()) {
                        hatch_holder->setAction(retract_arm_) ;
                        state_ = State::RetractArm ;
                    }
                    else {
                        auto lifter = getGamePiece().getLifter() ;
                        lifter->setAction(set_lifter_level_one_) ;
                        state_ = State::FinalHeight ;
                    }
                }
                break ;

            case State::RetractArm:
                if (retract_arm_->isDone()) {
                    auto lifter = getGamePiece().getLifter() ;
                    lifter->setAction(set_lifter_level_one_) ;
                    state_ = State::FinalHeight ;                                                
                }
                break ;

            case State::FinalHeight:
                if (set_lifter_level_one_->isDone()) 
                    state_ = State::Idle ;
                break;

            case State::Idle:
                break ;
            }
        }
        
        bool ResetIntakeAction::isDone() {
            return state_ == State::Idle ;
        }

        void ResetIntakeAction::cancel() {
        }

        std::string ResetIntakeAction::toString() {
            std::string ret("ResetIntakeAction") ;
            return ret ;
        }
    }
}