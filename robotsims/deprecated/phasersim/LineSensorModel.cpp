#include "LineSensorModel.h"
#include <frc/RobotSimBase.h>

using namespace frc ;
using namespace xero::sim ;
using namespace ctre::phoenix::motorcontrol::can ;

namespace xero {
    namespace sim {
        namespace phaser {
            LineSensorModel::LineSensorModel(RobotSimBase &simbase, const std::string &name) : SubsystemModel(simbase, name) {
                std::string param ; 
                
                param = "hw:linesensor:" + name + ":0" ;
                i0num_ = simbase.getSettingsParser().getInteger(param) ;
                i0state_ = false ;

                param = "hw:linesensor:" + name + ":1" ;
                i1num_ = simbase.getSettingsParser().getInteger(param) ;
                i1state_ = false ;

                param = "hw:linesensor:" + name + ":2" ;
                i2num_ = simbase.getSettingsParser().getInteger(param) ;
                i2state_ = false ;

                i0_ = nullptr ;
                i1_ = nullptr ;
                i2_ = nullptr ;
            }

            LineSensorModel::~LineSensorModel() {
            }

            bool LineSensorModel::processEvent(const std::string &event, int value) {
                bool ret = false ;
                if (event == "values") {
                    ret = true ;
                    if ((value & 1) != 0) {
                        i0state_ = false ;
                    }
                    else {
                        i0state_ = true ;
                    }

                    if ((value & 2) != 0) {
                        i1state_ = false ;
                    }
                    else {
                        i1state_ = true ;
                    }

                    if ((value & 4) != 0) {
                        i2state_ = false ;
                    }
                    else {
                        i2state_ = true ;
                    }

                    if (i0_ != nullptr)
                        i0_->SimulatorSetValue(i0state_) ;

                    if (i1_ != nullptr)
                        i1_->SimulatorSetValue(i1state_) ;

                    if (i2_ != nullptr)
                        i2_->SimulatorSetValue(i2state_) ;
                }

                return ret ;
            }

            void LineSensorModel::generateDisplayInformation(std::list<std::string> &lines) {

                std::string line ;

                line = "   State:" ;
                line += " "  ;                
                line += std::to_string(i0state_) ;

                line += " "  ;                
                line += std::to_string(i1state_) ;

                line += " "  ;                
                line += std::to_string(i2state_) ;

                lines.push_back(line) ;
            }

            std::string LineSensorModel::toString() {
                std::string result("linesensor: ") ;
                return result ;
            }

            void LineSensorModel::run(double dt) {
            }

            void LineSensorModel::addDevice(DigitalInput *input) {
                if (input->GetChannel() == i0num_) {
                    i0_ = input ;
                    input->SimulatorSetValue(true) ;
                }
                else if (input->GetChannel() == i1num_) {
                    i1_ = input ;
                    input->SimulatorSetValue(true) ;                    
                }
                else if (input->GetChannel() == i2num_) {
                    i2_ = input ;
                    input->SimulatorSetValue(true) ;                    
                }
            }
        }
    }
}
