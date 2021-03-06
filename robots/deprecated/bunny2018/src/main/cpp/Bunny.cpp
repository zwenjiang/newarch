
#include "Bunny.h"
#include "bunnysubsystem/BunnySubsystem.h"
#include "bunnyoi/BunnyOISubsystem.h"
#include "automodes/BunnyAutoMode.h"
#include "bunnyids.h"
#include <SequenceAction.h>
#include <basegroups.h>
#include <DelayAction.h>
#include <MessageLogger.h>
#include <frc/SmartDashboard/SmartDashboard.h>

#ifdef SIMULATOR
#include <Bunny2018Simulator.h>
#endif

#include <memory>
#include <cassert>

using namespace xero::misc ;
using namespace xero::base ;

namespace xero {
    namespace bunny2018 {

        //
        // Note, this name (bunny2018) must match the name where the source code is
        // located.  The robot data file will be found in that directory for simulation
        // and will be named with the give name with a dat extension.  For instance, for
        // bunny2018, the file will be bunny2018/bunny2018.dat
        //
        // The float point number given (0.02) is the duration of the robot loop.  In this
        // case we are running at a 20 millisecond robot loop.  The robot does everything it
        // needs to do in a single loop and then sleeps until it has used 20 ms for each loop.
        // These number are never exact, but we use them to get an approximately consistent
        // loop time.
        //
        Bunny::Bunny() : xero::base::Robot("bunny2018", 0.02) {
        }

        //
        // Return the top level subsysstem for the robot cast to the type for
        // this specific robot.
        //
        std::shared_ptr<BunnySubsystem> Bunny::getBunnySubsystem() {
            auto sub = getRobotSubsystem() ;
            return std::dynamic_pointer_cast<BunnySubsystem>(sub) ;
        }

        //
        // Chnage the calls to the message logger in this method to indicate what
        // messages are to be displayed. 
        //
        void Bunny::enableSpecificMessages() {
            MessageLogger& logger = getMessageLogger();

            //
            // Decide what message groups (incl. subsystems) you want to see
            //
            // logger.enableSubsystem(MSG_GROUP_TANKDRIVE);
            // logger.enableSubsystem(MSG_GROUP_ACTIONS);
            // logger.enableSubsystem(MSG_GROUP_PARSER) ;
            // logger.enableSubsystem(MSG_GROUP_OI) ;
            // logger.enableSubsystem(MSG_GROUP_SORTER) ;
            //
            logger.enableSubsystem(MSG_GROUP_SHOOTER_VERBOSE) ;         
        }

        void Bunny::RobotHardwareInit() {
            //
            // This is where the subsystems for the robot get created
            //
            auto robot_p = std::make_shared<BunnySubsystem>(*this) ;
            auto oi = robot_p->getOI() ;
            auto db = robot_p->getTankDrive() ;
            assert(oi != nullptr) ;
            assert(db != nullptr) ;
            setRobotSubsystem(robot_p, oi, db) ;
        }

        std::shared_ptr<ControllerBase> Bunny::createAutoController() {
            return std::make_shared<BunnyAutoMode>(*this) ;
        }
        
        std::shared_ptr<ControllerBase> Bunny::createTeleopController() {
            //
            // This is where the teleop controller is created
            //
            return nullptr ;
        }
         
        std::shared_ptr<ControllerBase> Bunny::createTestController() {
            //
            // This is where the test controller is created
            //
            return nullptr ;
        }

    }
}
