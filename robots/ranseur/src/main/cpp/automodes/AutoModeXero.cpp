#include "AutoModeXero.h"
#include <Ranseur.h>
#include <bunnyarm/BunnyArm.h>
#include <bunnyarm/BunnyArmDeployAction.h>
#include <tubcollector/TubCollector.h>
#include <tubcollector/TubCollectorTubAction.h>
#include <tubcollector/TubCollectorEjectTubAction.h>
#include <ranseurcameratracker/DriveByVisionAction.h>
#include <tubmanipulatorsubsystem/TubManipulatorCollectAction.h>
#include <tubmanipulatorsubsystem/TubManipulatorDumpAction.h>
#include <tankdrive/actions/TankDriveFollowPathAction.h>
#include <actions/DelayAction.h>
#include <actions/SequenceAction.h>
#include <actions/TerminateAction.h>
#include <memory>

using namespace xero::base ;

namespace xero
{
    namespace ranseur 
    {
        AutoModeXero::AutoModeXero(Robot &robot) : RanseurAutoModeBase(robot, "FetchTheTub", "Drive to center and pick up tub")
        {
            ActionPtr act ;
            auto &ranseur = dynamic_cast<Ranseur &>(getRobot()) ;
            auto bunnyarm = ranseur.getRanseurRobotSubsystem()->getBunnyArm() ;
            auto tubcollector = ranseur.getRanseurRobotSubsystem()->getTubManipulatorSubsystem()->getTubCollector() ;
            auto tubarm = ranseur.getRanseurRobotSubsystem()->getTubManipulatorSubsystem()->getTubArm() ;
            auto tubwrist = ranseur.getRanseurRobotSubsystem()->getTubManipulatorSubsystem()->getTubWrist() ;
            auto tankdrive = ranseur.getRanseurRobotSubsystem()->getTankDrive() ;
            auto camera = ranseur.getRanseurRobotSubsystem()->getCameraTracker() ;
            auto tubmanipulatorsubsytem = ranseur.getRanseurRobotSubsystem()->getTubManipulatorSubsystem() ;
            auto parallel = std::make_shared<ParallelAction>() ;
            
            // add parallel to sequence
            // everything else contained in parallel -> 2 seq actions
            pushAction(parallel) ;

            //// 1ST SEQUENCE ////
            auto sequence = std::make_shared<SequenceAction>(robot.getMessageLogger()) ;
            parallel->addAction(sequence) ;
            // TBD : terminate pathfollowing if sees the tub
            auto path = std::make_shared<TankDriveFollowPathAction>(*tankdrive, "BunnyAutoMode_PathToTub") ;
            auto term = std::make_shared<TerminateAction>(tankdrive, path , ranseur) ;
            term->addTerminator(camera) ; 
            sequence->pushAction(term) ;
            sequence->pushAction(std::make_shared<DriveByVisionAction>(*tankdrive, *camera)) ;

            //// 2ND SEQUENCE ////
            sequence = std::make_shared<SequenceAction>(robot.getMessageLogger()) ;
            parallel->addAction(sequence) ;
            sequence->pushAction(std::make_shared<DelayAction>(1.6)) ;
            sequence->pushAction(std::make_shared<BunnyArmDeployAction>(*bunnyarm, true)) ;
            sequence->pushAction(std::make_shared<DelayAction>(0.5)) ;
            sequence->pushAction(std::make_shared<TubManipulatorCollectAction>(*tubmanipulatorsubsytem)) ;
            sequence->pushAction(std::make_shared<TubManipulatorDumpAction>(*tubmanipulatorsubsytem)) ; 

           }

        AutoModeXero::~AutoModeXero()
        {            
        }
    }
}
