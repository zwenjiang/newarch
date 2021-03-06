#include "SingleMotorSubsystem.h"
#include "SingleMotorSubsystemAction.h"
#include "Robot.h"
#include "motors/MotorController.h"
#include "motors/MotorFactory.h"

using namespace xero::misc;

namespace xero {
    namespace base {
        SingleMotorSubsystem::SingleMotorSubsystem(Subsystem *parent, 
                                                   const std::string &name, 
                                                   uint64_t mid) : Subsystem(parent,name) 
        {
            msg_id_ = mid ;

            //
            // The hardware fields in the params file are derived from the subsystem name.  If the subsystem name is intake, then the
            // hardware fields in the parameters file will be hw:intake.
            //
            std::string motorparam = HWPrefix + name + ":motor" ;
            motor_ = getRobot().getMotorFactory()->createMotor(motorparam);
            current_power_ = 0.0 ;
        }

        SingleMotorSubsystem::SingleMotorSubsystem(Subsystem *parent, 
                                                   const std::string &name, 
                                                   std::shared_ptr<MotorController> motor, 
                                                   uint64_t mid) : Subsystem(parent,name) 
        {
            msg_id_ = mid ;
            motor_ = motor;
            motor_->setNeutralMode(MotorController::NeutralMode::Brake);
            current_power_ = 0.0 ;
        } 

        void SingleMotorSubsystem::setMotor(double power) 
        {
            motor_->set(power) ;
            current_power_ = power ;
        }       

        SingleMotorSubsystem::~SingleMotorSubsystem()
        {
        }

        bool SingleMotorSubsystem::canAcceptAction(ActionPtr action) 
        {          
            auto coll = std::dynamic_pointer_cast<SingleMotorSubsystemAction>(action) ;
            return coll != nullptr ;
        }

        void SingleMotorSubsystem::reset() 
        {
            Subsystem::reset() ;
            setMotor(0.0) ;
        }

        void SingleMotorSubsystem::init(LoopType ltype) 
        {
            Subsystem::init(ltype);
            motor_->reapplyInverted();
        }
    }
}