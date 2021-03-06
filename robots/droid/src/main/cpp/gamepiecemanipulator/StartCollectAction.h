#pragma once

#include "GamePieceManipulatorAction.h"
#include "actions/ParallelAction.h"

namespace xero {
    namespace droid {
        class StartCollectAction : public GamePieceManipulatorAction {
        public:
            StartCollectAction(GamePieceManipulator &subsystem);
            virtual ~StartCollectAction();

            void start() override ;
            void run() override ;
            void cancel() override ;

            std::string toString() override {
                return "StartCollectAction";
            }

        private:
            xero::base::ActionPtr conveyor_receive_action_ ;
        };
    }

}
