#pragma once

#include <Speedometer.h>
#include "singlemotorsubsystem/SingleMotorSubsystem.h"
#include "XeroEncoder.h"

namespace xero {
    namespace base {
        class MotorEncoderSubsystem : public SingleMotorSubsystem {
            friend class MotorEncoderGoToAction;
        public:
            /// \brief Create a new subsystem object
            /// \param parent a reference to the parent subsystem
            /// \param name the name of this subsystem, must be unique across all subsystems
            /// \param angular Whether the encoder is measuring an angle
            /// \param config the  base name of the parameter file entries that contains the subsystem definitions
            /// \param id the message logger id to use for messages from this class            
            /// \sa xero::base::MotorFactory
            /// \sa xero::misc::SettingsParser
            MotorEncoderSubsystem(Subsystem *parent, const std::string &name, const std::string config,
                                  uint64_t id, bool angular = false );

            virtual bool canAcceptAction(xero::base::ActionPtr action);
            virtual void computeState();

            double getPosition() { return speedometer_.getDistance(); }

            xero::misc::Speedometer &getSpeedometer() { return speedometer_; }
            bool isAngular() { return encoder_->isAngular(); }

            void calibrate();

            void reset() override;

        private:
            std::shared_ptr<XeroEncoder> encoder_;

            xero::misc::Speedometer speedometer_;

            const std::string configName_;
            uint64_t msg_id_;
        };
    }
}
