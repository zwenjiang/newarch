#include "CargoHolder.h"
#include "phaserids.h"
#include <Robot.h>
#include <SequenceAction.h>
#include <MessageLogger.h>

using namespace xero::base ;
using namespace xero::misc ;

namespace xero {
    namespace phaser {
        CargoHolder::CargoHolder(xero::base::Robot &robot, uint64_t id) : SingleMotorSubsystem(robot, "CargoHolder",  "hw:cargoholder:motor", id) {
            sensor_ = std::make_shared<frc::DigitalInput>(robot.getSettingsParser().getInteger("hw:cargoholder:sensor"));
            has_cargo_ = false ;
        }

        CargoHolder::~CargoHolder() {
        }   

        void CargoHolder::computeState() {
            has_cargo_ = sensor_->Get() ;

            auto &logger = getRobot().getMessageLogger() ;
            logger.startMessage(MessageLogger::MessageType::debug, MSG_GROUP_CARGO_HOLDER) ;
            logger << "CargoHolder:" ;
            logger << " HasCargo " << has_cargo_ ;
            logger.endMessage() ;                
        }
    }
}
