#pragma once

#include <$$robotname$$subsystem/$$RobotName$$Subsystem.h>
#include <tankdrive/TankDrive.h>
#include <Robot.h>
#include <frc/Compressor.h>

namespace xero {
    namespace $$robotname$$ {

        /// \brief the concrete class that represents the robot Phoenix
        class $$RobotName$$ : public xero::base::Robot {
        public:
            $$RobotName$$() ;

            /// \brief return the robot subsystem cast to the phoenix specific type
            /// \returns the robot subsystem cast to the phoenix specific type
            std::shared_ptr<$$RobotName$$Subsystem> get$$RobotName$$Subsystem() {
                return std::dynamic_pointer_cast<$$RobotName$$Subsystem>(getRobotSubsystem()) ;
            }

            virtual void loadPaths() ;

        protected:

            /// \brief called to initialize the robot, basically creating the subsystems
            virtual void RobotHardwareInit() ;

            /// \brief called to create the autonomous controller
            /// \returns a robot controller
            virtual std::shared_ptr<xero::base::ControllerBase> createAutoController() ;
            
            /// \brief called to create the teleop controller
            /// \returns a robot controller
            virtual std::shared_ptr<xero::base::ControllerBase> createTeleopController() ;

            /// \brief called to create the test controller
            /// \returns a robot controller
            virtual std::shared_ptr<xero::base::ControllerBase> createTestController() ;

            /// \brief enable specific message for logging
            virtual void enableSpecificMessages() ; 

            /// \brief returns true if the robot is the competition robot
            virtual bool isCompBot() ;

        private:
            bool comp_bot_flag_set_ ;
            bool comp_bot_ ;
        } ;
    }
}
