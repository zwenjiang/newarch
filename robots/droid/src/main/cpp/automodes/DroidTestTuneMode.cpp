#include "DroidTestTuneMode.h"
#include "Droid.h"
#include <actions/Action.h>
#include <actions/DelayAction.h>
#include <tankdrive/actions/TankDriveCharAction.h>
#include <tankdrive/actions/TankDriveScrubCharAction.h>
#include <tankdrive/actions/TankDriveAngleAction.h>
#include <motorencodersubsystem/MotorEncoderPowerAction.h>
#include <motorencodersubsystem/MotorEncoderGoToAction.h>
#include <singlemotorsubsystem/SingleMotorSubsystem.h>
#include <singlemotorsubsystem/SingleMotorPowerAction.h>

#include <turret/Turret.h>
#include <turret/FollowTargetAction.h>

#include <gamepiecemanipulator/GamePieceManipulator.h>
#include <gamepiecemanipulator/ShootTestingAction.h>
#include <gamepiecemanipulator/ShootAction.h>

#include <gamepiecemanipulator/intake/Intake.h>
#include <gamepiecemanipulator/intake/CollectOnAction.h>
#include <gamepiecemanipulator/intake/CollectOffAction.h>

#include <gamepiecemanipulator/conveyor/Conveyor.h>
#include <gamepiecemanipulator/conveyor/ConveyorEmitAction.h>
#include <gamepiecemanipulator/conveyor/ConveyorPrepareToEmitAction.h>
#include <gamepiecemanipulator/conveyor/ConveyorPrepareToReceiveAction.h>
#include <gamepiecemanipulator/conveyor/ConveyorReceiveAction.h>
#include <gamepiecemanipulator/conveyor/ConveyorStopAction.h>
#include <gamepiecemanipulator/conveyor/ConveyorOnAction.h>

#include <gamepiecemanipulator/shooter/Shooter.h>
#include <gamepiecemanipulator/shooter/ShooterVelocityAction.h>

using namespace xero::base;
using namespace xero::misc;

namespace xero
{
    namespace droid
    {
        DroidTestTuneMode::DroidTestTuneMode(xero::base::Robot &robot)
            : DroidAutoMode(robot, "TestMode", "Runs various tests based on the params file")
        {
            ActionPtr act ;
            auto &droid = dynamic_cast<Droid &>(getRobot()) ;
            auto tankdrive = droid.getDroidSubsystem()->getTankDrive() ;
            auto turret = droid.getDroidSubsystem()->getTurret();
            auto game = droid.getDroidSubsystem()->getGamePieceManipulator();

            int mode = robot.getSettingsParser().getInteger("auto:testmode:which");
            double dist = robot.getSettingsParser().getDouble("auto:testmode:distance");
            double power = robot.getSettingsParser().getDouble("auto:testmode:power");
            double duration = robot.getSettingsParser().getDouble("auto:testmode:duration");

            switch(mode)
            {
                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 0 - 9 drive base related
                //
                //////////////////////////////////////////////////////////////////////////////////////////
            case 0:     // Drive base straight characterization
                pushSubActionPair(tankdrive, std::make_shared<TankDriveCharAction>(*tankdrive, duration, power, true)) ;
                break;

            case 1:     // Drive base rotate characterization (note duration is total angle)
                pushSubActionPair(tankdrive, std::make_shared<TankDriveScrubCharAction>(*tankdrive, power, duration, true));
                break;

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 10 - 19 intake related
                //
                //////////////////////////////////////////////////////////////////////////////////////////
            case 10:
                pushSubActionPair(game->getIntake(), std::make_shared<MotorEncoderPowerAction>(*game->getIntake(), power, duration));
                break;

            case 11:     // Test the collector
                pushSubActionPair(game->getIntake(), std::make_shared<CollectOnAction>(*game->getIntake()), false) ;
                pushAction(std::make_shared<DelayAction>(droid.getMessageLogger(), 3.0));
                pushSubActionPair(game->getIntake(), std::make_shared<CollectOffAction>(*game->getIntake()));                
                break;

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 20 - 29 conveyor related
                //
                //////////////////////////////////////////////////////////////////////////////////////////
            case 20:     // Test the conveyor - just turn it on
                pushSubActionPair(game->getConveyor(), std::make_shared<ConveyorOnAction>(*game->getConveyor()));
                break;

            case 21:     // Test the conveyor - collect path
                pushSubActionPair(game->getConveyor(), std::make_shared<ConveyorPrepareToReceiveAction>(*game->getConveyor()));            
                pushSubActionPair(game->getConveyor(), std::make_shared<ConveyorReceiveAction>(*game->getConveyor()));   
                pushAction(std::make_shared<DelayAction>(droid.getMessageLogger(), 5.0)); 
                pushSubActionPair(game->getConveyor(), std::make_shared<ConveyorStopAction>(*game->getConveyor()));
                break;

            case 22:     // Test the conveyor - shoot path
                pushSubActionPair(game->getConveyor(), std::make_shared<ConveyorPrepareToEmitAction>(*game->getConveyor()));            
                pushSubActionPair(game->getConveyor(), std::make_shared<ConveyorEmitAction>(*game->getConveyor()));   
                pushAction(std::make_shared<DelayAction>(droid.getMessageLogger(), 5.0)); 
                pushSubActionPair(game->getConveyor(), std::make_shared<ConveyorStopAction>(*game->getConveyor()));
                break;

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 30 - 39 shooter related
                //
                //////////////////////////////////////////////////////////////////////////////////////////
            case 30:     // Test the shooter
                pushSubActionPair(game->getShooter(), std::make_shared<ShooterVelocityAction>(*game->getShooter(), 7000.0));
                pushAction(std::make_shared<DelayAction>(droid.getMessageLogger(), 5.0)); 
                pushSubActionPair(game->getShooter(), std::make_shared<ShooterVelocityAction>(*game->getShooter(), 0.0));                                
                break;

            case 31:     // Shoot balls
                pushSubActionPair(game, std::make_shared<ShootTestingAction>(*game));
                break;

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 40 - 49 turret related
                //
                //////////////////////////////////////////////////////////////////////////////////////////                
            case 40:
                pushSubActionPair(turret, std::make_shared<SingleMotorPowerAction>(*turret, power, duration));
                break;

            case 41:
                pushSubActionPair(turret, std::make_shared<MotorEncoderGoToAction>(*turret, dist));
                break;

            case 42:
                pushSubActionPair(turret, std::make_shared<FollowTargetAction>(*turret));
                break;

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 50 - 59 control panel spinner related
                //
                //////////////////////////////////////////////////////////////////////////////////////////

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 60 - 69 climber related
                //
                //////////////////////////////////////////////////////////////////////////////////////////

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 70 - 79 buddy climber related
                //
                //////////////////////////////////////////////////////////////////////////////////////////            

                //////////////////////////////////////////////////////////////////////////////////////////
                //
                // 100+ robot level system tests
                //
                //////////////////////////////////////////////////////////////////////////////////////////

            case 100:       // Complete shooting action
                pushSubActionPair(turret, std::make_shared<FollowTargetAction>(*turret));
                pushSubActionPair(game, std::make_shared<ShootAction>(*game));
                break;
            }
        }

        DroidTestTuneMode::~DroidTestTuneMode()
        {
        }
    }
}