#include "PhoenixAutoController.h"
#include "ThreeCubeAutoMode.h"
#include "PhoenixTestTuneMode.h"
#include <tankdrive/modes/StraightCharAutoMode.h>
#include <tankdrive/modes/FollowPathAutomode.h>

using namespace xero::base ;

namespace xero {
    namespace phoenix {
        PhoenixAutoController::PhoenixAutoController(Robot &robot) : AutoController(robot) {
        }
        
        PhoenixAutoController::~PhoenixAutoController() {
        }

        void PhoenixAutoController::updateAutoMode(int sel, const std::string &gamedata) {
            AutoModePtr mode = nullptr ;

            switch(sel) {
            case 0:
                mode = std::make_shared<ThreeCubeAutoMode>(getRobot(), ThreeCubeAutoMode::Side::Right);
                break ;

            case 1:
                mode = std::make_shared<ThreeCubeAutoMode>(getRobot(), ThreeCubeAutoMode::Side::Left);            
                break ;                
                
            case 2:
                mode = std::make_shared<FollowPathAutomode>(getRobot(), "TestPath_P1");
                break ;

            case 9:
                mode = std::make_shared<PhoenixTestTuneMode>(getRobot()) ;
                break ;
            }
            setAction(mode) ;
        }
    }
}
