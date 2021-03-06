#pragma once

#include "gamepiece.h"
#include "phaseroi/PhaserOISubsystem.h"
#include "cargoholder/CargoHolder.h"
#include "carloshatch/CarlosHatch.h"
#include "cargointake/CargoIntake.h"
#include "turntable/Turntable.h"
#include <lifter/Lifter.h>
#include <LoopType.h>
#include <Subsystem.h>

namespace xero {
    namespace phaser {

        class GamePieceManipulator : public xero::base::Subsystem {
        public:
            GamePieceManipulator(xero::base::Robot &robot) ;
            virtual ~GamePieceManipulator() ;

            std::shared_ptr<CargoHolder> getCargoHolder(){
                return cargo_holder_ ;
            } 

            std::shared_ptr<CarlosHatch> getHatchHolder(){
                return hatch_holder_ ;
            } 

            std::shared_ptr<CargoIntake> getCargoIntake() {
                return cargo_intake_ ;
            }

            std::shared_ptr<xero::base::Lifter> getLifter() {
                return lifter_ ;
            }

            std::shared_ptr<Turntable> getTurntable() {
                return turntable_ ;
            }

            virtual bool canAcceptAction(xero::base::ActionPtr action) ;
            virtual void run() ;
            virtual void init(xero::base::LoopType ltype) ;

            GamePieceType getGamePieceType() ;

        private:
            std::shared_ptr<CargoHolder> cargo_holder_ ;
            std::shared_ptr<CarlosHatch> hatch_holder_ ;
            std::shared_ptr<CargoIntake> cargo_intake_ ;
            std::shared_ptr<xero::base::Lifter> lifter_ ;
            std::shared_ptr<Turntable> turntable_ ;
        
        } ;
    }
}