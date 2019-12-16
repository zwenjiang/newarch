#include "TubArm.h"
#include "TubArmAction.h"
#include "ranseurids.h"
#include <Robot.h>
#include <actions/SequenceAction.h>
#include <frc/DigitalInput.h>

using namespace xero::base ;
using namespace xero::misc ;

namespace xero {
    namespace ranseur {
      
        TubArm::TubArm(Subsystem *parent) : MotorEncoderSubsystem(parent, "tubarm", "tubarm", MSG_GROUP_TUBARM, true) {
            setSmartDashboardName("TubArm") ;
        }

        TubArm::~TubArm() {
        }

        // sees whether or not 
        bool TubArm::canAcceptAction(ActionPtr action) {
            if (MotorEncoderSubsystem::canAcceptAction(action))
                return true ;

            auto tub_arm_act_p = std::dynamic_pointer_cast<TubArmAction>(action) ;
            return (tub_arm_act_p != nullptr);
        }

        void TubArm::computeState() {
            MotorEncoderSubsystem::computeState() ;
        }        

        void TubArm::init(LoopType lt) 
        {
            MotorEncoderSubsystem::init(lt) ;

            if (lt == LoopType::Autonomous)
            {
                double delta = 90 - getPosition()  ;
                getEncoder()->offsetBValue(delta) ;
            }
        }

        void TubArm::postHWInit()
        {
            MotorEncoderSubsystem::postHWInit() ;
            double delta = 90 - getPosition() ;
            getEncoder()->offsetBValue(delta) ;            
        }
    }
}
