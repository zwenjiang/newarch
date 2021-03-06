#include <LifterModel.h>
#include <frc/RobotSimBase.h>
#include <MessageLogger.h>
#include <cmath>

using namespace frc ;
using namespace xero::misc ;

namespace xero {
    namespace sim {
        LifterModel::LifterModel(RobotSimBase &simbase) : SubsystemModel(simbase, "lifter") {
            bottom_limit_height_ = simbase.getSettingsParser().getDouble("lifter:sim:bottom_limit_height") ;
            top_limit_height_ = simbase.getSettingsParser().getDouble("lifter:sim:top_limit_height") ;
            height_ = bottom_limit_height_ ;

            in_per_tick_ = simbase.getSettingsParser().getDouble("lifter:sim:inches_per_tick") ;

            inch_per_sec_per_volt_ = simbase.getSettingsParser().getDouble("lifter:sim:inches_per_sec_per_volt") ;
            gravity_equivalent_speed_ = simbase.getSettingsParser().getDouble("lifter:sim:gravity_equivalent_speed") ;
            encoder_base_ = 0  ;

            encoder_1_ = simbase.getSettingsParser().getInteger("hw:lifter:encoder1") ;
            encoder_2_ = simbase.getSettingsParser().getInteger("hw:lifter:encoder2") ;
            if (simbase.getSettingsParser().isDefined("hw:lifter:limit:bottom"))
                bottom_limit_channel_ = simbase.getSettingsParser().getInteger("hw:lifter:limit:bottom") ;
            else
                bottom_limit_channel_ = -1 ;

            if (simbase.getSettingsParser().isDefined("hw:lifter:limit:top"))
                top_limit_channel_ = simbase.getSettingsParser().getInteger("hw:lifter:limit:top") ;
            else
                top_limit_channel_ = -1 ;
            motor_1_ = simbase.getSettingsParser().getInteger("hw:lifter:motor:1") ;

            top_limit_ = nullptr ;
            bottom_limit_ = nullptr ;

            power_ = 0.0 ;

            enc_ = nullptr ;
            bottom_limit_ = nullptr ;
            top_limit_ = nullptr ;
        }

        LifterModel::~LifterModel() {
        }

        bool LifterModel::processEvent(const std::string &name, int value) {
            bool ret = false ;
            if (name == "verify") {
                double height = static_cast<double>(value) / 100.0 ;
                double err = std::fabs(height - height_) ;
                if (err > 2.0) {
                    MessageLogger &logger = getRobotMessageLogger() ;
                    logger.startMessage(MessageLogger::MessageType::error) ;
                    logger << "VerifyError: lifte" ;
                    logger << " expected " << height ;
                    logger << " actual " << height_ ;
                    logger.endMessage() ;
                }
                ret = true ;
            }

            return ret ;
        }        

        void LifterModel::generateDisplayInformation(std::list<std::string> &lines) {

            std::string line ;

            lines.push_back("  Height: " + std::to_string(getHeight())) ;
            lines.push_back("  Motor: " + std::to_string(getPower())) ;

            line = "  TopLimit: " ;
            line += (getTopLimit() ? "ON" : "OFF") ;
            lines.push_back(line) ;

            line = "  BottomLimit: " ;
            line += (getBottomLimit() ? "ON" : "OFF") ;
            lines.push_back(line) ;
        }

        std::string LifterModel::toString() {
            std::string result("lifter: ") ;

            result += "height " + std::to_string(height_) ;
            result += ", top " ;
            if (top_limit_ != nullptr)
                result += (top_limit_->Get() ? "ON" : "OFF") ;
            else
                result += "UNASSIGNED" ;

            result += ", bottom " ;
            if (bottom_limit_ != nullptr)
                result += (bottom_limit_->Get() ? "ON" : "OFF") ;
            else
                result += "UNASSIGNED" ;

            return result ;
        }

        void LifterModel::run(double dt) {
            double i = 99.0 ;

            if (power_ > 0.1) {
                i *= 2.0 ;
            }

            double dh = power_ * inch_per_sec_per_volt_* dt + gravity_equivalent_speed_ * dt ;
            height_ += dh ;

            if (height_ <= bottom_limit_height_) {
                height_ = bottom_limit_height_ ;
                if (bottom_limit_ != nullptr)
                    bottom_limit_->SimulatorSetValue(false) ;
            }
            else {
                if (bottom_limit_ != nullptr)
                    bottom_limit_->SimulatorSetValue(true) ;
            }

            if (height_ >= top_limit_height_) {
                height_ = top_limit_height_ ;
                if (top_limit_ != nullptr)
                    top_limit_->SimulatorSetValue(false) ;
            }
            else {
                if (top_limit_ != nullptr)
                    top_limit_->SimulatorSetValue(true) ;
            }

            int encval = static_cast<int>((height_ - bottom_limit_height_) / in_per_tick_) + encoder_base_ ;
            if (enc_ != nullptr)
                enc_->SimulatorSetValue(encval) ;
        }

        void LifterModel::inputChanged(SimulatedObject *obj) {
            
            std::lock_guard<std::mutex> lock(getLockMutex()) ;
            ctre::phoenix::motorcontrol::can::TalonSRX *talon = dynamic_cast<ctre::phoenix::motorcontrol::can::TalonSRX *>(obj) ;
            if (talon != nullptr)
                power_ = talon->Get() ;
        }
        
        void LifterModel::addDevice(ctre::phoenix::motorcontrol::can::TalonSRX *motor) {
            if (motor->GetDeviceID() == motor_1_) {
                motor1_ = motor ;
                motor1_->addModel(this) ;
            }
        }

        void LifterModel::addDevice(frc::Encoder *encoder) {
            int first, second ;

            encoder->SimulatorGetDigitalIOs(first, second) ;
            if ((first == encoder_1_ && second == encoder_2_) || (first == encoder_2_ && second == encoder_1_)) {
                enc_ = encoder ;
                enc_->addModel(this) ;
                enc_->SimulatorSetValue(encoder_base_) ;
            }
        }

        void LifterModel::addDevice(frc::DigitalInput *input)  {
            if (input->GetChannel() == top_limit_channel_) {
                top_limit_ = input ;
                top_limit_->addModel(this) ;
                top_limit_->SimulatorSetValue(true) ;
            }

            if (input->GetChannel() == bottom_limit_channel_) {
                bottom_limit_ = input ;
                bottom_limit_->addModel(this) ;
                bottom_limit_->SimulatorSetValue(true) ;
            }
        }
    }
}
