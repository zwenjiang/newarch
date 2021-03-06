#pragma once

#include <RobotSubsystem.h>
#include <tankdrive/TankDrive.h>
#include <singlemotorsubsystem/SingleMotorSubsystem.h>
#include <MessageListener/MessageListener.h>
#include <shooter/Shooter.h>

namespace xero {
    namespace bunny2018 {

        class BunnyOISubsystem ;

        class BunnySubsystem : public xero::base::RobotSubsystem {
        public:
            BunnySubsystem(xero::base::Robot &robot) ;
            virtual ~BunnySubsystem() ;

            std::shared_ptr<xero::base::TankDrive> getTankDrive() {
                return std::dynamic_pointer_cast<xero::base::TankDrive>(getDriveBase()) ;
            }

            std::shared_ptr<xero::base::SingleMotorSubsystem> getCollector() {
                return collector_ ;
            }            

            std::shared_ptr<xero::base::SingleMotorSubsystem> getHopper() {
                return hopper_ ;
            }   

            std::shared_ptr<xero::base::SingleMotorSubsystem> getIntake() {
                return intake_ ;
            }               

            std::shared_ptr<Shooter> getShooter() {
                return shooter_ ;
            }
            
            std::shared_ptr<BunnyOISubsystem> getOI() {
                return oi_ ;
            }
            
            std::shared_ptr<xero::base::MessageListener> getMessageListener() {
                return ml_ ;
            }

        private:
            std::shared_ptr<BunnyOISubsystem> oi_ ;
            std::shared_ptr<xero::base::MessageListener> ml_ ;
            std::shared_ptr<xero::base::SingleMotorSubsystem> collector_ ;
            std::shared_ptr<xero::base::SingleMotorSubsystem> hopper_ ;
            std::shared_ptr<xero::base::SingleMotorSubsystem> intake_ ;
            std::shared_ptr<Shooter> shooter_ ;
        } ;
    }
}
