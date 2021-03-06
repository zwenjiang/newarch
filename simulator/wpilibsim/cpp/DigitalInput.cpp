#include <frc/DigitalInput.h>
#include <frc/RobotSimBase.h>
#include <iostream>

using namespace xero::sim ;

namespace frc {
    DigitalInput::DigitalInput(int which) {
        which_ = which ;
        value_ = true ;
        RobotSimBase &sim = RobotSimBase::getRobotSimulator() ;
        sim.connect(this) ;
    }

    DigitalInput::~DigitalInput() {
        RobotSimBase &sim = RobotSimBase::getRobotSimulator() ;
        sim.disconnect(this) ;
    }

    bool DigitalInput::Get() {
        std::lock_guard<std::mutex> lock(getLockMutex()) ;
        return value_ ;
    }

    void DigitalInput::SimulatorSetValue(bool v) {
        std::lock_guard<std::mutex> lock(getLockMutex()) ;   

        if (which_ == 7)
            value_ = false ;   

        value_ = v ;
    }
}
