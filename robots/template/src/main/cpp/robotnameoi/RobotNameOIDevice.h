#pragma once

#include <oi/OIDevice.h> 
#include <Action.h>
#include <ActionSequence.h>

namespace xero {
    namespace $$robotname$$ {
        class $$RobotName$$OISubsystem ;

        class $$RobotName$$OIDevice : public xero::base::OIDevice {
        public:
            $$RobotName$$OIDevice($$RobotName$$OISubsystem &sub, int index) ;
            virtual ~$$RobotName$$OIDevice() ;

            virtual int getAutoModeSelector() {
                return getValue(automode_) ;
            }

            virtual void generateActions(xero::base::ActionSequence &seq) ;

            virtual void init() ;

        private:
            void bindOI() ;
            
        private:
            size_t automode_ ;
        } ;
    }
}