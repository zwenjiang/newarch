#pragma once

#include "actions/Action.h"
#include "basegroups.h"
#include "tankdrive/TankDrive.h"


/// \file


namespace xero {
    namespace base {
        class TankDrive;

        /// \brief This is the base class for all actions for the to be assigned to the TankDrive class.
        /// \sa TankDrive
        class TankDriveAction : public GenericAction {
        public:
            /// \brief Create a new TankDriveAction
            /// \param tank_drive the tank drive subsystem
            TankDriveAction(TankDrive &tank_drive) : GenericAction(tank_drive.getRobot().getMessageLogger()), tank_drive_(tank_drive) {
            }

        protected:
            /// \brief Return the tankdrive subsystem associated with this action
            /// \returns the tankdrive subsystem assocaited with this action
            TankDrive &getTankDrive() {
                return tank_drive_ ;
            }

            /// \brief set the motors to the percent power given
            /// \param left_percent percent to set for the left motor
            /// \param right_percent percent to set for the right motor
            void setMotorsToPercents(double left_percent, double right_percent){
                tank_drive_.setMotorsToPercents(left_percent, right_percent) ;
            }

        private:
            /// \brief The tank drive subsystem
            TankDrive &tank_drive_;
        };
    }
}
