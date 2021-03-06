#pragma once

#include <memory>
#include <motorencodersubsystem/MotorEncoderHoldAction.h>
#include <cameratracker/LimeLight.h>
#include "Turret.h"

namespace xero {
    namespace droid {
        class DroidLimeLight;
        class FollowTargetAction : public xero::base::MotorEncoderSubsystemAction {
        public:
            FollowTargetAction(Turret &subsystem);
           
            void start() override;
            void run() override;
            void cancel() override;

            std::string toString() override { return "FollowTargetAction"; }

            Turret &getTurret() { 
                return static_cast<Turret&>(getSubsystem());
            }

        private:
            double threshold_;
            xero::misc::PIDCtrl pid_ ;
            std::shared_ptr<DroidLimeLight> ll_ ;
        };
    }
}
