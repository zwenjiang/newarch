#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>
#include <functional>
#include <optional>

#include "MessageLogger.h"
#include "MotorController.h"
#include "SettingsParser.h"

namespace xero {
    namespace base {
        class Robot;
        class MotorController;

        class MotorFactory {
        public:
            /// \brief Creates a motor factory.
            MotorFactory(Robot &robot);

            typedef std::shared_ptr<MotorController> MotorPtr;


            /// \brief Creates a motor.
            /// \param configID The configuration file ID where the motor is defined.
            ///
            /// To define a single motor:
            ///     configID:canid    5         # The CAN bus ID of the motor.
            ///     configID:type     talon_srx # The type of the motor.
            /// To define a group of motors:
            ///     configID:1:canid  5
            ///     configID:1:type   talon_srx
            ///     configID:2:canid  6
            ///     configID:2:type   talon_srx
            ///     configID:2:invert true     # Inverts a single motor relative to the rest of the group
            ///     configID:invert   true     # Inverts the entire motor group
            ///     ...
            MotorPtr createMotor(std::string configID);

        private:
            enum MotorType { CAN, PWM } ;        
            struct MotorDescriptor
            {
                MotorDescriptor(MotorType t, std::function<MotorPtr(int)> fun)
                {
                    type_ = t ;
                    createFunction = fun ;
                }

                MotorType type_ ;
                std::function<MotorPtr(int)> createFunction ;
            } ;

        private:
            std::set<int> canIdsUsed_;
            std::set<int> pwmIdsUsed_;
            
            xero::misc::MessageLogger &messageLogger_;
            xero::misc::SettingsParser &settingsParser_;
            void handleError(std::string configID, std::string msg);

            static std::map<std::string, MotorDescriptor> motorConstructors_;

            MotorPtr createSingleMotor(std::string configID);
            
            bool isInverted(std::string configID);
            std::optional<MotorController::NeutralMode> getNeutralMode(std::string configID);
        };
    }
}