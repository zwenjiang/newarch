#include "PhaserAutoModeController.h"
#include "Phaser.h"
#include <tankdrive/TankDriveFollowPathAction.h>
#include <tankdrive/TankDriveCharAction.h>
#include <tankdrive/TankDriveScrubCharAction.h>
#include <tankdrive/LineDetectAction.h>
#include <tankdrive/LineFollowAction.h>
#include <frc/DriverStation.h>

using namespace xero::base ;

namespace xero {
    namespace phaser {
        PhaserAutoModeController::PhaserAutoModeController(Robot &robot) : AutoController(robot) {
        }
        
        PhaserAutoModeController::~PhaserAutoModeController() {
        }

        void PhaserAutoModeController::updateAutoMode(int sel, const std::string &gamedata) {
            ActionSequencePtr mode = nullptr ;

            switch(sel) {
            case 0:
                mode = createStraightCharAutoMode() ;
                break ;

            case 1:
                mode = createScrubCharAutoMode() ;
                break ;

            case 2:
                mode = createTestOne() ;
                break ;

            case 3:
                mode = createTestTwo() ;
                break ;

            case 4:
                mode = createFollowLine() ;
                break ;
            }
            setAction(mode) ;
        }

        ActionSequencePtr PhaserAutoModeController::createFollowLine() {
            std::string name = "Line Follow Test" ;
            std::string desc = "Test the line follower" ;
            ActionSequencePtr mode = std::make_shared<ActionSequence>(getRobot().getMessageLogger(), name, desc) ;
            ActionPtr act ;

            auto &phaser = dynamic_cast<Phaser &>(getRobot()) ;
            auto phaserrobot = phaser.getPhaserRobotSubsystem() ;
            auto db = phaserrobot->getTankDrive() ;
            auto ls = phaserrobot->getLightSensor() ;

            act = std::make_shared<LineDetectAction>(*ls, *db) ;
            mode->pushSubActionPair(db, act) ;

            act = std::make_shared<LineFollowAction>(*ls, *db, "linefollow:power", "linefollow:distance", "linefollow:adjust") ;
            mode->pushSubActionPair(db, act) ;            

            return mode ;         
        }             

        ActionSequencePtr PhaserAutoModeController::createStraightCharAutoMode() {
            std::string name = "Char Drive Base" ;
            std::string desc = "Characterize the drive base" ;
            ActionSequencePtr mode = std::make_shared<ActionSequence>(getRobot().getMessageLogger(), name, desc) ;
            ActionPtr act ;

            auto &phaser = dynamic_cast<Phaser &>(getRobot()) ;
            auto phaserrobot = phaser.getPhaserRobotSubsystem() ;
            auto db = phaserrobot->getTankDrive() ;

            act = std::make_shared<TankDriveCharAction>(*db, 3.0, 0.5) ;
            mode->pushSubActionPair(db, act) ;

            return mode ;         
        }        

        ActionSequencePtr PhaserAutoModeController::createScrubCharAutoMode() {
            std::string name = "Char Drive Base" ;
            std::string desc = "Characterize the drive base" ;
            ActionSequencePtr mode = std::make_shared<ActionSequence>(getRobot().getMessageLogger(), name, desc) ;
            ActionPtr act ;

            auto &phaser = dynamic_cast<Phaser &>(getRobot()) ;
            auto phaserrobot = phaser.getPhaserRobotSubsystem() ;
            auto db = phaserrobot->getTankDrive() ;

            act = std::make_shared<TankDriveScrubCharAction>(*db, 3.0, 0.3, 0.5) ;
            mode->pushSubActionPair(db, act) ;

            return mode ;         
        }  

        ActionSequencePtr PhaserAutoModeController::createTestOne() {
            std::string name = "Test One" ;
            std::string desc = "Drive straight" ;
            ActionSequencePtr mode = std::make_shared<ActionSequence>(getRobot().getMessageLogger(), name, desc) ;
            ActionPtr act ;

            auto &phaser = dynamic_cast<Phaser &>(getRobot()) ;
            auto phaserrobot = phaser.getPhaserRobotSubsystem() ;
            auto db = phaserrobot->getTankDrive() ;

            act = std::make_shared<TankDriveFollowPathAction>(*db, "TestOne") ;
            mode->pushSubActionPair(db, act) ;

            return mode ;         
        }

        ActionSequencePtr PhaserAutoModeController::createTestTwo() {
            std::string name = "Test Two" ;
            std::string desc = "S Curve" ;
            ActionSequencePtr mode = std::make_shared<ActionSequence>(getRobot().getMessageLogger(), name, desc) ;
            ActionPtr act ;

            auto &phaser = dynamic_cast<Phaser &>(getRobot()) ;
            auto phaserrobot = phaser.getPhaserRobotSubsystem() ;
            auto db = phaserrobot->getTankDrive() ;

            act = std::make_shared<TankDriveFollowPathAction>(*db, "SCurve") ;
            mode->pushSubActionPair(db, act) ;

            return mode ; 
        }        
    }
}