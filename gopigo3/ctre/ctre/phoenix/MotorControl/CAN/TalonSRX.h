#pragma once


#include "../SensorCollection.h"
#include "../ControlMode.h"
#include "../NeutralMode.h"
#include "../IMotorController.h"
#include <cassert>

namespace ctre {
    namespace phoenix {
        namespace motorcontrol {
            namespace can {

                class TalonSRX : public IMotorController {
                public:
                    TalonSRX(int id) : IMotorController(id) {
                    }

                    virtual ~TalonSRX() {        
                    }

                    ctre::phoenix::motorcontrol::SensorCollection &GetSensorCollection() {
                        return sensors_ ;
                    }

                private:
                    ctre::phoenix::motorcontrol::SensorCollection sensors_ ;
                } ;
            }
        }
    }
}
