#include "Shooter.h"
#include "droidids.h"
#include "ShooterAction.h"
#include "Droid.h"
#include <tankdrive/TankDrive.h>
#include <motors/MotorGroupController.h>

using namespace xero::misc;
using namespace xero::base;

namespace xero {
    namespace droid {
        Shooter::Shooter(Subsystem *parent): MotorEncoderSubsystem(parent, "shooter", MSG_GROUP_SHOOTER) {
#ifdef ALL_SMART_DASHBOARD
            setSmartDashboardName("shooter") ;
#endif
            setReadyToFire(false);

            auto &settings = getRobot().getSettingsParser();
            int which = settings.getInteger("hw:shooter:hood") ;
            hoodServo_ = std::make_shared<frc::Servo>(which);

            std::string hoodConfig = "shooter:hood:";
            hoodUpPos_ = settings.getDouble(hoodConfig + "up");
            hoodDownPos_ = settings.getDouble(hoodConfig + "down");
            getMotorController()->setCurrentLimit(40) ;
            getMotorController()->setNeutralMode(MotorController::NeutralMode::Coast) ;

            hood_down_speed_ = settings.getDouble("shooter:down_speed") ;
            hood_down_delay_ = settings.getDouble("shooter:down_delay") ;

            actual_ = HoodPosition::Unknown ;
            desired_ = HoodPosition::Down ;
        }

        void Shooter::run()
        {
            MotorEncoderSubsystem::run() ;
            updateHood() ;
        }

        bool Shooter::canAcceptAction(xero::base::ActionPtr action) {
            return MotorEncoderSubsystem::canAcceptAction(action) || 
                std::dynamic_pointer_cast<ShooterAction>(action) != nullptr;
        }

        void Shooter::setHood(HoodPosition pos) {
            desired_ = pos ;
            updateHood() ;
        }

        bool Shooter::isHoodReady()
        {
            return getRobot().getTime() - change_time_ > hood_down_delay_ ;
        }
        
        void Shooter::updateHood()
        {
            Droid &droid = dynamic_cast<Droid &>(getRobot()) ;
            auto db = droid.getDriveBase() ;

            if (abs(db->getVelocity()) > hood_down_speed_ || actual_ == HoodPosition::Unknown)
            {
                if (actual_ != HoodPosition::Down)
                {
                    hoodServo_->Set(hoodDownPos_) ;
                    actual_ = HoodPosition::Down ;
                }
            }
            else if (desired_ != actual_)
            {
                if (desired_ == HoodPosition::Down)
                    hoodServo_->Set(hoodDownPos_) ;
                else
                    hoodServo_->Set(hoodUpPos_) ;

                change_time_ = getRobot().getTime() ;
                actual_ = desired_ ;
            }
        }
    }
}