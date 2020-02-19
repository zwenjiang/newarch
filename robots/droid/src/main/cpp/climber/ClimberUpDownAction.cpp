#include "ClimberUpDownAction.h"
#include <singlemotorsubsystem/SingleMotorPowerAction.h>

using namespace xero::base ;

namespace xero
{
    namespace droid
    {
        ClimberUpDownAction::ClimberUpDownAction(Climber &subsystem, double p) : ClimberAction(subsystem)
        {
            action_ = std::make_shared<SingleMotorPowerAction>(*subsystem.getLifter(), p) ;
        }

        ClimberUpDownAction::ClimberUpDownAction(Climber &subsystem, const std::string &name) : ClimberAction(subsystem)
        {
            action_ = std::make_shared<SingleMotorPowerAction>(*subsystem.getLifter(), name) ;
        }

        ClimberUpDownAction::~ClimberUpDownAction()
        {            
        }

        void ClimberUpDownAction::start()
        {
            getSubsystem().getLifter()->setAction(action_) ;
        }

        void ClimberUpDownAction::run()
        {
        }

        std::string ClimberUpDownAction::toString()
        {
            std::string ret = "ClimberUpDownAction " ;
            return ret ;
        }
    }
}