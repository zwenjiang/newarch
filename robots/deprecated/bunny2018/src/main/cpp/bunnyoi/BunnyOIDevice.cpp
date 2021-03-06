#include "BunnyOIDevice.h"
#include "BunnyOISubsystem.h"
#include "Bunny.h"
#include "bunnysubsystem/BunnySubsystem.h"
#include "shooter/ShooterEjectOneBallAction.h"
#include "shooter/ShooterStageBallAction.h"
#include <SequenceAction.h>
#include <singlemotorsubsystem/SingleMotorSubsystem.h>
#include <singlemotorsubsystem/SingleMotorPowerAction.h>

using namespace xero::base ;

namespace xero {
    namespace bunny2018 {
        BunnyOIDevice::BunnyOIDevice(BunnyOISubsystem &sub, int index) : OIDevice(sub, index) {
            initialize() ;
        }

        BunnyOIDevice::~BunnyOIDevice() {
        }
        
        void BunnyOIDevice::initialize() {
            //
            // Actions
            //
            collector_ = mapAxisSwitch(4,3) ;
            hopper_ = mapAxisSwitch(3,3) ;
            shoot_one_ = mapButton(3, OIButton::ButtonType::LowToHigh) ;
            shoot_many_ = mapButton(6, OIButton::ButtonType::Level) ;
            test_auto_modes_ = mapButton(12, OIButton::ButtonType::LowToHigh) ;     
            intake_on_off_ = mapButton(16, OIButton::ButtonType::Level) ;

            //
            // Modes
            //
            std::vector<double> mapping = { -0.9, -0.75, -0.5, -0.25, 0, 0.2, 0.4, 0.6, 0.8, 1.0 } ;
            automode_ = mapAxisScale(6, mapping) ;                                  // Rotary knob, 10 position

            base_ = 0 ;
        }

        
        int BunnyOIDevice::getAutoModeSelector() {
            if (getValue(test_auto_modes_)) {
                base_ = 10 - base_ ;
            }

            int res = getValue(automode_) + base_  ;
            return res ;
        }

        void BunnyOIDevice::createActions() {
            Bunny &bunny = dynamic_cast<Bunny &>(getSubsystem().getRobot()) ;
            auto collector = bunny.getBunnySubsystem()->getCollector() ;
            auto hopper = bunny.getBunnySubsystem()->getHopper() ;
            auto shooter = bunny.getBunnySubsystem()->getShooter() ;
            auto intake = bunny.getBunnySubsystem()->getIntake() ;

            collector_fwd_action_ = std::make_shared<SingleMotorPowerAction>(*collector, "collector:power:fwd") ;
            collector_off_action_ = std::make_shared<SingleMotorPowerAction>(*collector, 0.0) ;
            collector_rev_action_ = std::make_shared<SingleMotorPowerAction>(*collector, "collector:power:rev") ;
            hopper_fwd_action_ = std::make_shared<SingleMotorPowerAction>(*hopper, "hopper:power:fwd") ;
            hopper_off_action_ = std::make_shared<SingleMotorPowerAction>(*hopper, 0.0) ;
            hopper_rev_action_ = std::make_shared<SingleMotorPowerAction>(*hopper, "hopper:power:rev") ;
            shoot_one_action_ = std::make_shared<ShooterEjectOneBallAction>(*shooter) ;
            shoot_many_action_on_ = std::make_shared<SingleMotorPowerAction>(*shooter, "shooter:power:continous") ;
            shoot_many_action_stage_ = std::make_shared<ShooterStageBallAction>(*shooter) ;

            intake_on_action_ = std::make_shared<SingleMotorPowerAction>(*intake, "intake:power:fwd") ;
            intake_off_action_ = std::make_shared<SingleMotorPowerAction>(*intake, 0.0) ;           
        }

        void BunnyOIDevice::generateActions(SequenceAction &seq) {
            if (collector_fwd_action_ == nullptr)
                createActions() ;

            Bunny &bunny = dynamic_cast<Bunny &>(getSubsystem().getRobot()) ;
            auto collector = bunny.getBunnySubsystem()->getCollector() ;
            auto hopper = bunny.getBunnySubsystem()->getHopper() ;
            auto shooter = bunny.getBunnySubsystem()->getShooter() ;
            auto intake = bunny.getBunnySubsystem()->getIntake() ;

            switch(getValue(collector_)){
                case 0:
                    seq.pushSubActionPair(collector, collector_rev_action_) ;
                    break;
                case 1:
                    seq.pushSubActionPair(collector, collector_off_action_) ;                
                    break;
                case 2:
                    seq.pushSubActionPair(collector, collector_fwd_action_) ;
                    break;
            }

            switch(getValue(hopper_)){
                case 0:
                    seq.pushSubActionPair(hopper, hopper_rev_action_) ;
                    break;
                case 1:
                    seq.pushSubActionPair(hopper,hopper_off_action_) ;
                    break;
                case 2:
                    seq.pushSubActionPair(hopper, hopper_fwd_action_) ;                
                    break;
            }

            if (getValue(intake_on_off_))
            {
                if (intake->getAction() == nullptr || intake->getAction() != intake_on_action_)
                    seq.pushSubActionPair(intake, intake_on_action_) ;
            }
            else
            {
                if (intake->getAction() == nullptr || intake->getAction() != intake_off_action_)
                    seq.pushSubActionPair(intake, intake_off_action_) ;
            }

            if (getValue(shoot_one_)) {
                seq.pushSubActionPair(shooter, shoot_one_action_) ;
            }
            else {
                if (shooter->getAction() == nullptr || shooter->getAction()->isDone()) {
                    if (getValue(shoot_many_)) {
                        seq.pushSubActionPair(shooter, shoot_many_action_on_) ;
                        shooter->clearBallIsStaged() ;
                    }
                    else {
                        seq.pushSubActionPair(shooter, shoot_many_action_stage_) ;
                    }
                }
            }
        }
    }
}
