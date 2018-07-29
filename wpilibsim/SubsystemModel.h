#pragma once

#include "TalonSRX.h"
#include "Encoder.h"
#include "DigitalInput.h"
#include "DriverStation.h"
#include "Solenoid.h"
#include "VictorSP.h"
#include "Timer.h"
#include "AHRS.h"
#include <mutex>
#include <map>

namespace xero {
    namespace sim {
        class RobotSimBase ;

        class SubsystemModel {
        public:
            SubsystemModel(RobotSimBase &simbase, const std::string &name) ;
            virtual ~SubsystemModel() ;

            virtual void run(double dt) = 0 ;
            virtual void inputChanged(SimulatedObject *obj) = 0 ;
            virtual std::string toString() = 0 ;

            virtual void addTalon(ctre::phoenix::motorcontrol::can::TalonSRX *talon)  {                
            }

            virtual void addEncoder(frc::Encoder *encoder) {                
            }

            virtual void addDigitalInput(frc::DigitalInput *input) {                
            }

            virtual void addDriverStation(frc::DriverStation *station) {
            }

            virtual void addSolenoid(frc::Solenoid *sol) {
            }

            virtual void addVictorSP(frc::VictorSP *victor) {
            }

            virtual void addTimer(frc::Timer *timer) {                
            }

            virtual void addNavX(AHRS *navx) {                
            }

            std::mutex &getLockMutex() ;

            const std::string &getName() const {
                return name_ ;
            }

            RobotSimBase &getSimulator() {
                return simbase_ ;
            }

        private:
            std::string name_ ;
            RobotSimBase &simbase_ ;
        } ;
    }
}