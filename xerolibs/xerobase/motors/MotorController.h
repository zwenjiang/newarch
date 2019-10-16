#pragma once

#include <memory>

/// \file

namespace xero {
    namespace base {
        class MotorController {
        public:
            enum class NeutralMode { Coast, Brake };

            /// \brief Create a new motor object
            MotorController() {}

            virtual ~MotorController() {};

            /// \brief Sets the motor's power.
            /// \param percent The power to set, in the range [-1, 1].
            virtual void set(double percent) = 0;

            /// \brief Sets whether the motor is inverted.
            /// \param inverted Whether the motor should be inverted.
            /// \warning If called after \c follow, the behavior is unspecified.
            virtual void setInverted(bool inverted) = 0;

            /// \brief Sets whether the motor will brake or coast.
            /// \param neutralMode The desired neutral mode.
            virtual void setNeutralMode(NeutralMode neutralMode) = 0;

            /// \brief Sets this motor to follow another.
            /// \param motor The motor to follow. Should assert if `motor` cannot be followed.
            /// \param invert Whether to invert the output of the leader.
            virtual void follow(std::shared_ptr<MotorController> motor, bool invert = false) = 0;
        };
}
}