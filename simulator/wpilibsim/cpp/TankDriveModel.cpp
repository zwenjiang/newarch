#include <frc/TankDriveModel.h>
#include <frc/RobotSimBase.h>
#include <xeromath.h>
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>

using namespace frc ;
using namespace xero::misc ;
using namespace xero::math ;
using namespace ctre::phoenix::motorcontrol::can ;

namespace xero {
    namespace sim {
        TankDriveModel::TankDriveModel(RobotSimBase &simbase) : SubsystemModel(simbase, "tankdrive") {
            inited_ = false ;

            left_ = 0.0;
            right_ = 0.0;
            angle_ = 0.0;
            navx_offset_ = 0.0 ;
            left_volts_ = 0.0;
            right_volts_ = 0.0;
            time_interval_ = 100000.0;
            last_output_ = 0.0;            
            xpos_ = 0.0 ;
            ypos_ = 0.0 ;
            last_xpos_ = 0.0 ;
            last_ypos_ = 0.0 ;
            speed_ = 0.0 ; 
            current_left_rps_ = 0.0 ;
            current_right_rps_ = 0.0 ;

            ticks_per_rev_ = simbase.getSettingsParser().getInteger("tankdrive:sim:ticks_per_rev") ;
            diameter_ = simbase.getSettingsParser().getDouble("tankdrive:sim:diameter") ;
            scrub_ = simbase.getSettingsParser().getDouble("tankdrive:sim:scrub") ;
            width_ = simbase.getSettingsParser().getDouble("tankdrive:sim:width") ;
            left_right_error_ = simbase.getSettingsParser().getDouble("tankdrive:sim:error_per_side") ;

            calcLowLevelParams(simbase) ;
            lowGear() ;

            left_enc_ = nullptr ;
            right_enc_ = nullptr ;
            navx_ = nullptr ;

            left_volts_ = 0.0 ;
            right_volts_ = 0.0 ;
        }

        TankDriveModel::~TankDriveModel() {
        }

        void TankDriveModel::calcLowLevelParams(RobotSimBase &simbase) {
            double maxhighvel = simbase.getSettingsParser().getDouble("tankdrive:high:maxvelocity") ;
            double maxhighaccel = simbase.getSettingsParser().getDouble("tankdrive:high:maxacceleration") ;
            double maxlowvel = simbase.getSettingsParser().getDouble("tankdrive:low:maxvelocity") ;
            double maxlowaccel = simbase.getSettingsParser().getDouble("tankdrive:low:maxacceleration") ;

            double circum = diameter_ * PI ;

            high_rps_per_volt_per_time_ = maxhighvel / circum ;
            low_rps_per_volt_per_time_ = maxlowvel / circum ;
            high_max_change_ = maxhighaccel / circum ;
            low_max_change_ = maxlowaccel / circum ;
        }

        void TankDriveModel::lowGear() {
            if (left_right_error_ > 1e-6) {
                std::uniform_real_distribution<double> unif(0.95, 1.05) ;
                std::default_random_engine re ;
                right_rps_per_volt_per_time_ = low_rps_per_volt_per_time_ * unif(re) ;
                left_rps_per_volt_per_time_ = low_rps_per_volt_per_time_ * unif(re) ;
                current_max_change_ = low_max_change_ ;                
            }
            else {
                right_rps_per_volt_per_time_ = low_rps_per_volt_per_time_  ;
                left_rps_per_volt_per_time_ = low_rps_per_volt_per_time_ ;
                current_max_change_ = low_max_change_ ;
            }

            gear_ = true ;
        }

        void TankDriveModel::highGear() {
            if (left_right_error_ > 1e-6) {
                std::uniform_real_distribution<double> unif(0.95, 1.05) ;
                std::default_random_engine re ;
                right_rps_per_volt_per_time_ = high_rps_per_volt_per_time_ * unif(re) ;
                left_rps_per_volt_per_time_ = high_rps_per_volt_per_time_ * unif(re) ;
                current_max_change_ = high_max_change_ ;                
            }
            else {
                right_rps_per_volt_per_time_ = high_rps_per_volt_per_time_  ;
                left_rps_per_volt_per_time_ = high_rps_per_volt_per_time_ ;
                current_max_change_ = high_max_change_ ;                  
            }

            gear_ = false ;
        }

        std::string TankDriveModel::toString() {
            std::string result("TankDrive: ") ;

            result += "leftv " + std::to_string(left_volts_) ;
            result += ", rightv " + std::to_string(right_volts_) ;
            result += ", left " + std::to_string(left_) ;
            result += ", right " + std::to_string(right_) ;
            result += ", angle " + std::to_string(angle_) ;
            result += ", leftenc " + std::to_string(left_enc_value_) ;
            result += ", rightenc " + std::to_string(right_enc_value_) ;
            result += ", xpos " + std::to_string(xpos_ ) ;
            result += ", ypos " + std::to_string(ypos_ ) ;

            return result ;
        }

        void TankDriveModel::init() {
            if (getSimulator().hasProperty("pos")) {
                std::vector<double> values ;
                const std::string &prop = getSimulator().getProperty("pos") ;

                if (parseDoubleList(prop, values) && values.size() == 2) {
                    xpos_ = values[0] ;
                    ypos_ = values[1] ;
                    last_xpos_ = xpos_ ;
                    last_ypos_ = ypos_ ;
                }
            }
        }

        double TankDriveModel::capValue(double prev, double target, double maxchange) {
            double ret ;

            if (target > prev) {
                if (target > prev + maxchange)
                    ret = prev + maxchange ;
                else
                    ret = target ;
            }
            else {
                if (target < prev - maxchange)
                    ret = prev - maxchange ;
                else
                    ret = target ;
            }

            return ret ;
        }

        void TankDriveModel::run(double dt) {
            bool b = true ;

            if (std::fabs(left_volts_) > 0.1 || std::fabs(right_volts_) > 0.1) {
                b = false ;
            }

            //
            // Calculate the new desired revolutions per second (RPS)
            //
            double desired_left_rps = -left_volts_ * left_rps_per_volt_per_time_ ;
            double desired_right_rps = right_volts_ * right_rps_per_volt_per_time_ ;

            //
            // Now, see how much our RPS can actually changes
            //
            current_left_rps_ = capValue(current_left_rps_, desired_left_rps, current_max_change_) ;
            current_right_rps_ = capValue(current_right_rps_, desired_right_rps, current_max_change_) ;

            //
            // Now, calculate distance each wheel moved based on the actual RPS
            //
            double dleft = current_left_rps_* dt * diameter_ * PI;
            double dright = current_right_rps_ * dt * diameter_ * PI;

            //
            // And add to the total distance so far
            //
            left_ += dleft;
            right_ += dright;

            double dv = (dleft - dright) / 2 * scrub_;

            double lrevs = left_ / (PI * diameter_);
            double rrevs = right_ / (PI * diameter_) ;

            updatePosition(dleft, dright, angle_) ;
            angle_ = normalizeAngleRadians(angle_ + (dv * 2.0) / width_) ;

            double dist = std::sqrt((xpos_ - last_xpos_) * (xpos_ - last_xpos_) + (ypos_ - last_ypos_) * (ypos_ - last_ypos_)) ;
            speed_ = dist / dt ;

            last_xpos_ = xpos_ ;
            last_ypos_ = ypos_ ;

            left_enc_value_ = static_cast<int32_t>(lrevs * ticks_per_rev_) ;
            right_enc_value_ = static_cast<int32_t>(rrevs * ticks_per_rev_) ;

            if (left_enc_ != nullptr)
                left_enc_->SimulatorSetValue(-left_enc_value_) ;

            if (right_enc_ != nullptr)
                right_enc_->SimulatorSetValue(right_enc_value_) ;

            if (navx_ != nullptr) {
                double deg = normalizeAngleDegrees(rad2deg(angle_ + navx_offset_)) ;
                navx_->SimulatorSetYaw(deg) ;
            }
        }

        void TankDriveModel::inputChanged(SimulatedObject *obj) {
            std::lock_guard<std::mutex> lock(getLockMutex()) ;

            TalonSRX *talon = dynamic_cast<TalonSRX *>(obj) ;
            if (talon != nullptr) {
                auto itl = std::find(std::begin(left_motors_), std::end(left_motors_), talon) ;
                if (itl != std::end(left_motors_)) {
                    left_volts_ = talon->Get() ;
                }

                auto itr = std::find(std::begin(right_motors_), std::end(right_motors_), talon) ;
                if (itr != std::end(right_motors_)) {
                    right_volts_ = talon->Get() ;
                }
            }
            else if (obj == left_enc_) {
                left_ = 0.0 ;
            }
            else if (obj == right_enc_) {
                right_ = 0.0 ;
            }
            else if (obj == navx_) {
                navx_offset_ = -angle_ ;
            }
            else if (obj == shifter_) {
                Solenoid *sol = dynamic_cast<Solenoid *>(obj) ;
                if (sol->Get() != gear_) {
                    if (sol->Get())
                        lowGear() ;
                    else
                        highGear() ;
                }
            }
        }

        void TankDriveModel::addTalonSRX(TalonSRX *motor) {
            if (motor->GetDeviceID() == 1 || motor->GetDeviceID() == 2 || motor->GetDeviceID() == 3) {
                left_motors_.push_back(motor) ;
                motor->addModel(this) ;
            }
            else if (motor->GetDeviceID() == 4 || motor->GetDeviceID() == 5 || motor->GetDeviceID() == 6) {
                right_motors_.push_back(motor) ;
                motor->addModel(this) ;
            }
        }

        void TankDriveModel::addEncoder(Encoder *encoder) {
            int first, second ;
            encoder->SimulatorGetDigitalIOs(first, second) ;
            if (first == 0 && second == 1) {
                left_enc_ = encoder ;
                left_enc_->addModel(this) ;
            }
            else if (first == 2 && second == 3) {
                right_enc_ = encoder ;
                right_enc_->addModel(this) ;
            }
        }

        void TankDriveModel::addNavX(AHRS *navx) {
            navx_ = navx ;
            navx_->addModel(this) ;
        }

        void TankDriveModel::addSolenoid(Solenoid *sol) {
            if (sol->SimulatorGetChannel() == 0) {
                shifter_ = sol ;
                shifter_->addModel(this) ;
            }
        }

        void TankDriveModel::updatePosition(double dl, double dr, double angle) {
            if (std::fabs(dl - dr) < 1e-6) {
                // Straight line
                xpos_ += dl * std::cos(angle) ;
                ypos_ += dr * std::sin(angle) ;
            }
            else {
                double r = width_ * (dl + dr) / (2 * (dr - dl)) ;
                double wd = (dr - dl) / width_ ;
                xpos_ = xpos_ + r * std::sin(wd + angle) - r * std::sin(angle) ;
                ypos_ = ypos_ - r * std::cos(wd + angle) + r * std::cos(angle) ;
            }
        }
    }
}
