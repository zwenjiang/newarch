#include "FireAction.h"

#include <SettingsParser.h>
#include <frc/smartdashboard/SmartDashboard.h>

#include "Droid.h"
#include "droidsubsystem/DroidSubsystem.h"
#include "conveyor/Conveyor.h"
#include "conveyor/ConveyorEmitAction.h"
#include "shooter/Shooter.h"
#include "shooter/ShooterVelocityAction.h"
#include "targettracker/TargetTracker.h"
#include "turret/Turret.h"

using namespace xero::base;
using namespace xero::misc;
namespace xero {
    namespace droid {
        std::vector<std::string> FireAction::columns_ =
        {
            "time",

            "is_firing",
            "ready",
            "ready_except_shooter",

            "limelight_ready",
            "turret_ready",
            "shooter_ready",
            "drivebase_ready",
            
            "conveyor_state",
            "turret_error",
            "shooter_velocity",
        } ;

        FireAction::FireAction(GamePieceManipulator &manip): 
            GamePieceManipulatorAction(manip),
            droidSubsystem_(*static_cast<Droid&>(manip.getRobot()).getDroidSubsystem()) {

            auto conveyor = getSubsystem().getConveyor();
            auto shooter = getSubsystem().getShooter();
            shooterVelocityAction_ = std::make_shared<ShooterVelocityAction>(*shooter, 0, false);
            conveyorEmitAction_ = std::make_shared<ConveyorEmitAction>(*conveyor);

            hoodIsDown_ = false;
            
            auto &settings = manip.getRobot().getSettingsParser();
            std::string drivebaseThresholdKey = "gamepiecemanipulator:fire:max_drivebase_velocity";
            assert(settings.isDefined(drivebaseThresholdKey) &&
                   "gamepiecemanipulator:fire:max_drivebase_velocity must be defined");
            drivebaseVelocityThreshold_ = settings.getDouble(drivebaseThresholdKey);
            
            std::string cameraThresholdKey = "gamepiecemanipulator:fire:max_camera_sample_age";
            assert(settings.isDefined(cameraThresholdKey) &&
                   "gamepiecemanipulator:fire:max_camera_sample_age must be defined");
            cameraSampleAgeThreshold_ = settings.getDouble(cameraThresholdKey);
            
            std::string hoodDown = "shooter:aim:hood_down:";
            std::string hoodUp = "shooter:aim:hood_up:";
            hoodDown_a_ = settings.getDouble(hoodDown + "a");
            hoodDown_b_ = settings.getDouble(hoodDown + "b");
            hoodDown_c_ = settings.getDouble(hoodDown + "c");
            
            hoodUp_a_ = settings.getDouble(hoodUp + "a");
            hoodUp_b_ = settings.getDouble(hoodUp + "b");
            hoodUp_c_ = settings.getDouble(hoodUp + "c");

            maxHoodUpDistance_ = settings.getDouble("shooter:aim:max_hood_up");
            minHoodDownDistance_ = settings.getDouble("shooter:aim:min_hood_down");

            plotid_ = getSubsystem().initPlot("FireAction") ;
        }

        void FireAction::start() {
            GamePieceManipulatorAction::start();
            auto shooter = getSubsystem().getShooter();
            isFiring_ = false;
            shooterVelocityAction_->setTarget(0);               // just set it to a known value,
            shooter->setAction(shooterVelocityAction_, true);   // we'll update it every tick

            startTime_ = getSubsystem().getRobot().getTime();
            getSubsystem().getRobot().getPlotManager().startPlot(plotid_, columns_);
        }

        void FireAction::setTargetVelocity() {
            auto tracker = droidSubsystem_.getTargetTracker();
            double dist = tracker->getDistance();

            if (dist > maxHoodUpDistance_) hoodIsDown_ = true;
            else if (dist < minHoodDownDistance_) hoodIsDown_ = false;

            double a, b, c;
            if (hoodIsDown_) {
                a = hoodDown_a_;
                b = hoodDown_b_;
                c = hoodDown_c_;
            } else {
                a = hoodUp_a_;
                b = hoodUp_b_;
                c = hoodUp_c_;
            }

            shooterVelocityAction_->setHood(hoodIsDown_);
            shooterVelocityAction_->setTarget(a*dist*dist + b*dist + c);
        
        }

        void FireAction::run() {
            GamePieceManipulatorAction::run();
            auto tracker = droidSubsystem_.getTargetTracker();
            auto turret = droidSubsystem_.getTurret();
            auto drivebase = droidSubsystem_.getTankDrive();
            auto conveyor = getSubsystem().getConveyor();
            auto shooter = getSubsystem().getShooter();

            setTargetVelocity();
            
            double sampleAge = getSubsystem().getRobot().getTime() - tracker->getLastCameraSampleTime();

            bool trackerReady = sampleAge < cameraSampleAgeThreshold_;
            bool turretReady = turret->isReadyToFire();
            bool shooterReady = shooter->isReadyToFire();
            bool drivebaseReady = abs(drivebase->getVelocity()) < drivebaseVelocityThreshold_;

            frc::SmartDashboard::PutBoolean("ShooterReady", shooterReady) ;
            frc::SmartDashboard::PutBoolean("TurretReady", turretReady) ;
            frc::SmartDashboard::PutBoolean("DrivebaseReady", drivebaseReady) ;
            frc::SmartDashboard::PutBoolean("trackerReady", trackerReady) ;

            bool readyToFireExceptShooter = trackerReady && drivebaseReady;
            bool readyToFire = trackerReady && drivebaseReady && turretReady && shooterReady;

            auto &logger = getMessageLogger();

            if (isFiring_) {
                // If we're out of balls, stop firing
                if (conveyor->isEmpty()) {
                    logger.startMessage(MessageLogger::MessageType::debug);
                    logger << "FireAction: Out of balls; done firing.";
                    logger.endMessage();
                    getSubsystem().getRobot().getPlotManager().endPlot(plotid_);
                    setDone();
                    stopChildActions();
                }

                // If we lose the taret, stop firing
                // Don't check the shooter because we expect some variance in speed as we
                // fire. The shooter should stabalize between shots. If it doesn't, then
                // we should add a delay between shots in ConveyorEmitAction.
                if (!readyToFireExceptShooter) {
                    logger.startMessage(MessageLogger::MessageType::debug);
                    logger << "FireAction: Lost target; waiting.";
                    logger.endMessage();
                    conveyorEmitAction_->stopFiring();
                    isFiring_ = false;
                }
            } else { // We're not currently firing.
                // If we're out of balls, stop trying to fire
                if (conveyor->isEmpty()) {
                    logger.startMessage(MessageLogger::MessageType::debug);
                    logger << "FireAction: Out of balls; done trying to fire.";
                    logger.endMessage();
                    getSubsystem().getRobot().getPlotManager().endPlot(plotid_);
                    setDone();
                    stopChildActions();
                }
                else if (readyToFire && !conveyor->isBusy()) {
                    // fire!
                    logger.startMessage(MessageLogger::MessageType::debug);
                    logger << "FireAction: Firing!";
                    logger.endMessage();
                    conveyor->setAction(conveyorEmitAction_, true);
                    isFiring_ = true;
                }
            }

            // Print a debug message
            logger.startMessage(MessageLogger::MessageType::debug);
            logger << "FireAction: isFiring: " << isFiring_ << "; ";
            if (readyToFire) logger << "ready to fire; ";
            else { 
                logger << "not ready to fire [";
                logger << "camera: " << (trackerReady ? "ready" : "waiting") << ", ";
                logger << "turret: " << (turretReady ? "ready" : "waiting") << ", ";
                logger << "shooter: " << (shooterReady ? "ready" : "waiting") << ", ";
                logger << "drivebase: " << (drivebaseReady ? "ready" : "waiting") << ", ";
                logger << "]; ";
                
            }
            logger << "hood: " << (hoodIsDown_ ? "down" : "up");
            logger.endMessage();

            getSubsystem().getRobot().getPlotManager().addPlotData(plotid_, {
                getSubsystem().getRobot().getTime() - startTime_,

                (double)isFiring_,
                (double)readyToFire,
                (double)readyToFireExceptShooter,
                
                (double)trackerReady,
                (double)turretReady,
                (double)shooterReady,
                (double)drivebaseReady,

                (double)conveyorEmitAction_->getStateIndex(),
                tracker->getRelativeAngle(),
                shooter->getSpeedometer().getVelocity()
            });
        }

        void FireAction::cancel() {
            Action::cancel();
            getSubsystem().getRobot().getPlotManager().endPlot(plotid_);
            stopChildActions();
            setDone();
        }

        void FireAction::stopChildActions() {
            shooterVelocityAction_->cancel();
            conveyorEmitAction_->cancel();
        }
    }
}