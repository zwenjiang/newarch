#include "LiftingCollector.h"
#include "phlifter/Lifter.h"
#include "phlifter/LifterGoToHeightAction.h"
#include "collector/Collector.h"
#include "collector/CollectCubeAction.h"
#include "LiftingCollectCubeAction.h"
#include <ActionSequence.h>
#include <Robot.h>

using namespace xero::base ;
using namespace xero::misc ;

namespace xero {
    namespace phoenix {
        LiftingCollector::LiftingCollector(Robot &robot) : Subsystem(robot, "liftingcollector") {

            lifter_ = std::make_shared<Lifter>(robot) ;
            addChild(lifter_) ;
            lifter_->createNamedSequences() ;
            
            collector_ = std::make_shared<Collector>(robot) ;
            addChild(collector_) ;
            collector_->createNamedSequences() ;
        }

        LiftingCollector::~LiftingCollector() {
        }

        void LiftingCollector::createNamedSequences() {
        }

        bool LiftingCollector::canAcceptAction(ActionPtr action) {
            auto ldir_p = std::dynamic_pointer_cast<LiftingCollectCubeAction>(action) ;
            if (ldir_p == nullptr)
                return false ;

            return true ;
        }

        void LiftingCollector::computeState() {
            lifter_->computeState() ;
            collector_->computeState() ;
        }
    }
}