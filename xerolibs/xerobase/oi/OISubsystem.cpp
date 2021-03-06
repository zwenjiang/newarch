#include "oi/OISubsystem.h"
#include "Robot.h"
#include "actions/SequenceAction.h"
#include "tankdrive/TankDrive.h"
#include "basegroups.h"
#include "DriverGamepadRumbleAction.h"
#include <MessageLogger.h>

using namespace xero::misc ;

namespace xero {
    namespace base {
        OISubsystem::OISubsystem(Subsystem *parent, const std::string &name, bool adddriver) 
                                : Subsystem(parent, name) {
            inited_ = false ;
            if (adddriver) {
                int driver = getRobot().getSettingsParser().getInteger("hw:driverstation:hid:driver") ;                
                driver_ = std::make_shared<DriverGamepad>(*this, driver);
                addHIDDevice(driver_) ;
            }

            std::string sequence_name = "OIsubsystem";
        }

        OISubsystem::~OISubsystem() {
        }     

        void OISubsystem::postHWInit() {
            Subsystem::postHWInit() ;
            for(auto dev: hiddevices_)
                dev->init() ;            
        }

        void OISubsystem::init(xero::base::LoopType ltype) {
            if (ltype == LoopType::OperatorControl) {
                for(auto dev: hiddevices_)
                    dev->init() ;
            }
        }

        int OISubsystem::getAutoModeSelector() {
            for(auto dev: hiddevices_) {
                int automode = dev->getAutoModeSelector() ;
                if (automode != -1)
                    return automode  ;
            }

            return -1 ;
        }

        void OISubsystem::computeState() {
            if (!inited_) {
                auto sub = getRobot().getDriveBase() ;
                std::shared_ptr<TankDrive> db = std::dynamic_pointer_cast<TankDrive>(sub) ;
                if (db != nullptr) {
                    driver_->init(db) ;
                    inited_ = true ;
                }
            }
            for(auto dev: hiddevices_)
                dev->computeState() ;
        }

        void OISubsystem::generateActions(SequenceActionPtr seq) {
            for(auto dev: hiddevices_)
            {
                if (dev->isEnabled())
                    dev->generateActions(*seq) ;
            }
        }

        void OISubsystem::run() {
        }

        bool OISubsystem::canAcceptAction(ActionPtr act) {
            std::shared_ptr<DriverGamepadRumbleAction> dact = std::dynamic_pointer_cast<DriverGamepadRumbleAction>(act) ;
            return dact != nullptr ;
        }
    }
}
