#include "Ranseur.h"
#include "ranseurids.h"
#include "automodes/RanseurAutoModeController.h"
#include <actions/DelayAction.h>
#include <TeleopController.h>
#include <basegroups.h>
#include <MessageDestDS.h>
#include <MessageLogger.h>
#include <MessageDestSeqFile.h>
#include <MessageDestStream.h>
#include <frc/DriverStation.h>
#include <frc/DigitalInput.h>
#include <memory>
#include <cassert>

using namespace xero::misc ;
using namespace xero::base ;

namespace xero {
    namespace ranseur {

        Ranseur::Ranseur() : xero::base::Robot("ranseur", 0.02) {   
            comp_bot_flag_set_ = false ;
        }

        bool Ranseur::isCompBot() {
            //
            // For the practice bot, we have a jumper between digital IO 6 and ground.  For the
            // competition bot we have nothing and the roborio pulls this value up to one.  In this
            // way we detect the difference between the COMP bot and the PRACTICE bot.
            //
            if (!comp_bot_flag_set_) {
                frc::DigitalInput input(6) ;
                comp_bot_ = input.Get() ;
            }
            return comp_bot_ ;
        }

        void Ranseur::enableSpecificMessages() {
            MessageLogger& logger = getMessageLogger();
                        
            //
            // Decide what message groups (incl. subsystems) you want to see
            //
            // logger.enableSubsystem(MSG_GROUP_TANKDRIVE);
            // logger.enableSubsystem(MSG_GROUP_TANKDRIVE_VERBOSE);            
            // logger.enableSubsystem(MSG_GROUP_ACTIONS);
            // logger.enableSubsystem(MSG_GROUP_ACTIONS_VERBOSE);               
            // logger.enableSubsystem(MSG_GROUP_PARSER) ;
            // logger.enableSubsystem(MSG_GROUP_OI) ;
            // logger.enableSubsystem(MSG_GROUP_ALL) ;         
            // logger.enableSubsystem(MSG_GROUP_CAMERA_TRACKER); 
            // logger.enableSubsystem(MSG_GROUP_CAMERA_TRACKER_VERBOSE);             
            // logger.enableSubsystem(MSG_GROUP_VISION_DRIVING); 
            // logger.enableSubsystem(MSG_GROUP_VISION_TERMINATOR);             
            // logger.enableSubsystem(MSG_GROUP_RANSEUR_OI) ;  
            // logger.enableSubsystem(MSG_GROUP_PARSER) ;   
            // logger.enableSubsystem(MSG_GROUP_TUBARM) ;
            // logger.enableSubsystem(MSG_GROUP_TUBWRIST) ;       
            // logger.enableSubsystem(MSG_GROUP_TUBCOLLECTOR) ;                            
            // logger.enableSubsystem(MSG_GROUP_ALL) ;                              

            logger.enableSubsystem(MSG_GROUP_OI) ;
            logger.enableSubsystem(MSG_GROUP_ACTIONS);               
            logger.enableSubsystem(MSG_GROUP_ACTIONS_VERBOSE);      

            //
            // This should stay on.  It will have no effect on the real robot
            // but will show information about injected events in the simulation
            // environment.
            //
            logger.enableSubsystem(MSG_GROUP_SIMULATOR) ;
        }
        
        void Ranseur::loadPaths() {
            auto paths = getPathManager() ;

#ifdef OLD_PATH_NAMES

#else
            paths->setExtensions("_left.csv", "_right.csv") ;
            
            paths->loadPath("BunnyAutoMode_PathToTub") ;
            paths->loadPath("Test_TestPath") ;
#endif
        }
        
        void Ranseur::RobotHardwareInit() {
            auto sub_p = std::make_shared<RanseurRobotSubsystem>(*this) ;
            setRobotSubsystem(sub_p, sub_p->getOI(), sub_p->getTankDrive()) ;
        }

        std::shared_ptr<ControllerBase> Ranseur::createAutoController() {
            auto ctrl = std::make_shared<RanseurAutoModeController>(*this) ;
            return ctrl ;
        }
        
        std::shared_ptr<ControllerBase> Ranseur::createTeleopController() {
            //
            // The base teleop controller is sufficient
            //
            return std::make_shared<xero::base::TeleopController>(*this) ;
        }
         
        std::shared_ptr<ControllerBase> Ranseur::createTestController() {
            //
            // This is where the test controller is created
            //
            return nullptr ;
        }
    }
}


