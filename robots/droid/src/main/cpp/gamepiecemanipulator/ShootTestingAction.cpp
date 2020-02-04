#include "ShootTestingAction.h"
#include "shooter/ShooterVelocityAction.h"
#include "conveyor/ConveyorPrepareToReceiveAction.h"
#include "conveyor/ConveyorPrepareToEmitAction.h"
#include "conveyor/ConveyorEmitAction.h"
#include "conveyor/ConveyorReceiveAction.h"
#include <gamepiecemanipulator/shooter/Shooter.h>
#include <gamepiecemanipulator/GamePieceManipulator.h>
#include <frc/smartdashboard/SmartDashboard.h>

namespace xero {
    namespace droid {

        static frc::SimpleWidget makeWidget() {
            wpi::StringMap<std::shared_ptr<nt::Value>> propmap;
            propmap.insert(std::make_pair("min", nt::Value::MakeDouble(0.0)));
            propmap.insert(std::make_pair("max", nt::Value::MakeDouble(6000.0)));
            return frc::Shuffleboard::GetTab("SmartDashboard").Add("Velocity", static_cast<double>(0.0)).WithWidget(frc::BuiltInWidgets::kNumberSlider).WithProperties(propmap);
        }

        ShootTestingAction::ShootTestingAction(GamePieceManipulator &subsystem) : GamePieceManipulatorAction(subsystem), widget_(makeWidget())
        {
            fire_ = std::make_shared<ShooterVelocityAction>(*getSubsystem().getShooter(), 0.0);            
        }

        ShootTestingAction::~ShootTestingAction()
        {
        }

        void ShootTestingAction::start() 
        {
            auto conveyor = getSubsystem().getConveyor() ;
            auto shooter = getSubsystem().getShooter();

            state_ = FireState::WaitPrepareReceive ;
            fire_->setTarget(0.0);
            shooter->setAction(fire_);
            conveyor->setAction(std::make_shared<ConveyorPrepareToReceiveAction>(*conveyor)) ;            
        }

        void ShootTestingAction::run() 
        {
            auto conveyor = getSubsystem().getConveyor() ;
            auto shooter = getSubsystem().getShooter();
            double current = fire_->getTarget();
            double target = widget_.GetEntry().GetDouble(current);

            if (std::fabs(current - target) > 10)
                fire_->setTarget(target);
            frc::SmartDashboard::PutBoolean("ReadyShoot", shooter->isReadyToFire()) ;

            switch(state_)
            {
            case FireState::WaitPrepareReceive:
                if (!conveyor->isBusy())
                {
                    state_ = FireState::WaitReceive ;
                    conveyor->setAction(std::make_shared<ConveyorReceiveAction>(*conveyor)) ;
                }
                break ;

            case FireState::WaitReceive:
                if (!conveyor->isBusy())
                {
                    state_ = FireState::WaitPrepareShoot ;
                    conveyor->setAction(std::make_shared<ConveyorPrepareToEmitAction>(*conveyor)) ;
                }
                break ;

            case FireState::WaitPrepareShoot:
                if (!conveyor->isBusy())
                {
                    state_ = FireState::WaitShoot ;
                    conveyor->setAction(std::make_shared<ConveyorEmitAction>(*conveyor)) ;
                }
                break ;

            case FireState::WaitShoot:
                if (!conveyor->isBusy())
                {
                    state_ = FireState::WaitPrepareReceive ;
                    conveyor->setAction(std::make_shared<ConveyorPrepareToReceiveAction>(*conveyor)) ;
                }
                break ;
            }
        }
    }
}