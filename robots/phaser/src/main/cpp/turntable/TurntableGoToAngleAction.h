#pragma once

#include "TurntableAction.h"
#include "Turntable.h"
#include <PIDACtrl.h>
#include <TrapezoidalProfile.h>

namespace xero {
    namespace phaser {
        class TurntableGoToAngleAction : public TurntableAction {
        public:
            TurntableGoToAngleAction(Turntable &turntable, double target) ;
            TurntableGoToAngleAction(Turntable &turntable, const std::string &name) ;
            virtual ~TurntableGoToAngleAction() ;

            virtual void start() ;
            virtual void run() ;
            virtual bool isDone() ;
            virtual void cancel() ;
            virtual std::string toString() ;

        private:
            double getAngleDifference(double start, double end) ;

        private:
            bool is_done_ ;
            double target_ ;
            double output_ ;
            double start_time_ ;

            double min_keepout_angle_ ;
            double max_keepout_angle_ ;
            
            double threshold_ ;
            std::shared_ptr<xero::misc::PIDACtrl> ctrl_ ;
            std::shared_ptr<xero::misc::TrapezoidalProfile> profile_ ;
            double start_angle_ ;
            size_t index_ ;

            int plotid_ ;
            static std::list<std::string> plot_columns_ ;  
        } ;
    }
}