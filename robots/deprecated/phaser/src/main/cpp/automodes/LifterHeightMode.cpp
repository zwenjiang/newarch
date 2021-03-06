#include "automodes/LifterHeightMode.h"
#include "Phaser.h"
#include <lifter/LifterGoToHeightAction.h>
#include <lifter/LifterPowerAction.h>
#include <DelayAction.h>

using namespace xero::base ;
using namespace xero::misc ;

namespace xero {
    namespace phaser {
        LifterHeightMode::LifterHeightMode(Robot &robot) : PhaserAutoModeBase(robot, "GoToHeight", "GoToHeight")
        {
            auto &phaser = dynamic_cast<Phaser &>(getRobot()) ;
            auto phaserrobot = phaser.getPhaserRobotSubsystem() ;
            auto lifter = phaserrobot->getGameManipulator()->getLifter() ;

            pushSubActionPair(lifter, std::make_shared<LifterPowerAction>(*lifter, 0.5)) ;
            pushAction(std::make_shared<DelayAction>(2.0)) ;
            pushSubActionPair(lifter, std::make_shared<LifterPowerAction>(*lifter, 0.0)) ;            
        }

        LifterHeightMode::~LifterHeightMode()
        {            
        }
    }
}