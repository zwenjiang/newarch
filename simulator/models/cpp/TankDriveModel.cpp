#include <xeromath.h>
#include <TankDriveModel.h>
#include <frc/RobotSimBase.h>
#include <xeromath.h>
#include <MessageLogger.h>
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>
#include <algorithm>

using namespace frc ;
using namespace xero::misc ;
using namespace xero::math ;
using namespace ctre::phoenix::motorcontrol::can ;
using namespace rev ;

namespace xero {
    namespace sim {
        TankDriveModel::TankDriveModel(RobotSimBase &simbase, MotorType mt) : SubsystemModel(simbase, "tankdrive") 
        {
            mt_ = mt ;
            left_ = 0.0;
            right_ = 0.0;
            angle_ = 0.0;
            last_angle_ = 0.0;
            total_angle_ = 0.0;
            navx_offset_ = 0.0;
            left_power_ = 0.0;
            right_power_ = 0.0;
            xpos_ = 100.0 ;
            ypos_ = 100.0 ;
            last_xpos_ = 0.0 ;
            last_ypos_ = 0.0 ;
            speed_ = 0.0 ; 
            current_left_rps_ = 0.0 ;
            current_right_rps_ = 0.0 ;

            getMotorParams(simbase) ;
            calcLowLevelParams(simbase) ;
            lowGear() ;

            left_enc_ = nullptr ;
            right_enc_ = nullptr ;
            left_spark_encoder_ = nullptr ;
            right_spark_encoder_ = nullptr ;
            navx_ = nullptr ;

            left_power_ = 0.0 ;
            right_power_ = 0.0 ;
        }

        TankDriveModel::~TankDriveModel() 
        {
        }

        void TankDriveModel::getMotorParams(RobotSimBase &simbase)
        {
            std::string config, var ;
            int i ;
            SettingsParser &settings = simbase.getSettingsParser() ;

            config = "hw:tankdrive:left:motor:" ;
            i = 1 ;
            while (true)
            {
                var = config + std::to_string(i) ;
                if (!settings.isDefined(var))
                    break ;

                left_motors_.push_back(settings.getInteger(var)) ;

                i++ ;
            }

            config = "hw:tankdrive:left:motor:invert" ;
            if (settings.isDefined(config) && settings.getBoolean(config) == true)
                left_motor_mult_ = -1.0 ;
            else
                left_motor_mult_ = 1.0 ;

            left_encoder_1_ = settings.getInteger("hw:tankdrive:left:encoder:1") ;
            left_encoder_2_ = settings.getInteger("hw:tankdrive:left:encoder:2") ;       

            config = "hw:tankdrive:left:encoder:invert" ;
            if (settings.isDefined(config) && settings.getBoolean(config) == true)
                left_encoder_mult_ = -1 ;
            else
                left_encoder_mult_ = 1 ;

            config = "hw:tankdrive:right:motor:" ;
            i = 1 ;
            while (true)
            {
                var = config + std::to_string(i) ;
                if (!settings.isDefined(var))
                    break ;

                right_motors_.push_back(settings.getInteger(var)) ;

                i++ ;
            }

            config = "hw:tankdrive:right:motor:invert" ;
            if (settings.isDefined(config) && settings.getBoolean(config) == true)
                right_motor_mult_ = -1.0 ;
            else
                right_motor_mult_ = 1.0 ;

            right_encoder_1_ = settings.getInteger("hw:tankdrive:right:encoder:1") ;
            right_encoder_2_ = settings.getInteger("hw:tankdrive:right:encoder:2") ;  

            config = "hw:tankdrive:right:encoder:invert" ;
            if (settings.isDefined(config) && settings.getBoolean(config) == true)
                right_encoder_mult_ = -1 ;
            else
                right_encoder_mult_ = 1 ;              

            assert(left_motors_.size() == right_motors_.size()) ;
            assert(left_motors_.size() > 0) ;

            ticks_per_rev_ = simbase.getSettingsParser().getInteger("tankdrive:sim:ticks_per_rev") ;
            diameter_ = simbase.getSettingsParser().getDouble("tankdrive:sim:diameter") ;
            scrub_ = simbase.getSettingsParser().getDouble("tankdrive:sim:scrub") ;
            width_ = simbase.getSettingsParser().getDouble("tankdrive:sim:width") ;
            length_ = simbase.getSettingsParser().getDouble("tankdrive:sim:length") ;
            left_right_error_ = simbase.getSettingsParser().getDouble("tankdrive:sim:error_per_side") ;

            //
            // If this drive base has a shifting gear box
            //
            shifter_channel_ = -1 ;
            if (simbase.getSettingsParser().isDefined("hw:tankdrive:shifter"))
                shifter_channel_ = simbase.getSettingsParser().getInteger("hw:tankdrive:shifter") ;
        }

        bool TankDriveModel::processEvent(const std::string &name, int value) {
            bool ret = false ;
            if (name == "xpos") {
                double pos = static_cast<double>(value) / 100.0 ;
                xpos_ = pos ;
                last_xpos_ = pos ;
                ret = true ;
            }
            else if (name == "ypos") {
                double pos = static_cast<double>(value) / 100.0 ;
                ypos_ = pos ;
                last_ypos_ = pos ;
                ret = true ;
            }            
            else if (name == "angle") {
                double pos = static_cast<double>(value) / 100.0 ;
                angle_ = pos ;
                ret = true ;                
            }

            return ret ;
        }                

        void TankDriveModel::generateDisplayInformation(std::list<std::string> &lines) {
            double d = xero::math::rad2deg(getAngle()) ;

            lines.push_back("  X: " + std::to_string(getXPos())) ;
            lines.push_back("  Y: " + std::to_string(getYPos())) ;
            lines.push_back("  Angle: " + std::to_string(d)) ;
            lines.push_back("  Speed: " + std::to_string(getSpeed())) ;
            lines.push_back("  LeftPos: " + std::to_string(left_)) ;
            lines.push_back("  RightPos: " + std::to_string(right_)) ;
            lines.push_back("  LeftPower: " + std::to_string(left_power_)) ;
            lines.push_back("  RightPower: " + std::to_string(right_power_)) ;
            if (robot_text_.length() > 0)
                lines.push_back("  Display: " + robot_text_);
        }

        void TankDriveModel::calcLowLevelParams(RobotSimBase &simbase) {
            double maxhighvel = simbase.getSettingsParser().getDouble("tankdrive:high:maxvelocity") ;
            double maxhighaccel = simbase.getSettingsParser().getDouble("tankdrive:high:maxacceleration") ;
            double maxlowvel = simbase.getSettingsParser().getDouble("tankdrive:low:maxvelocity") ;
            double maxlowaccel = simbase.getSettingsParser().getDouble("tankdrive:low:maxacceleration") ;

            double circum = diameter_ * PI ;

            high_rps_per_power_per_time_ = maxhighvel / circum ;
            low_rps_per_power_per_time_ = maxlowvel / circum ;
            high_max_change_ = maxhighaccel / circum ;
            low_max_change_ = maxlowaccel / circum ;
        }

        void TankDriveModel::lowGear() {
            if (left_right_error_ > 1e-6) {
                std::uniform_real_distribution<double> unif(0.95, 1.05) ;
                std::default_random_engine re ;
                right_rps_per_power_per_time_ = low_rps_per_power_per_time_ * unif(re) ;
                left_rps_per_power_per_time_ = low_rps_per_power_per_time_ * unif(re) ;
                current_max_change_ = low_max_change_ ;                
            }
            else {
                right_rps_per_power_per_time_ = low_rps_per_power_per_time_  ;
                left_rps_per_power_per_time_ = low_rps_per_power_per_time_ ;
                current_max_change_ = low_max_change_ ;
            }

            gear_ = Gear::LowGear ;
        }

        void TankDriveModel::highGear() {
            if (left_right_error_ > 1e-6) {
                std::uniform_real_distribution<double> unif(0.95, 1.05) ;
                std::default_random_engine re ;
                right_rps_per_power_per_time_ = high_rps_per_power_per_time_ * unif(re) ;
                left_rps_per_power_per_time_ = high_rps_per_power_per_time_ * unif(re) ;
                current_max_change_ = high_max_change_ ;                
            }
            else {
                right_rps_per_power_per_time_ = high_rps_per_power_per_time_  ;
                left_rps_per_power_per_time_ = high_rps_per_power_per_time_ ;
                current_max_change_ = high_max_change_ ;                  
            }

            gear_ = Gear::HighGear ;
        }

        std::string TankDriveModel::toString() {
            std::string result("TankDrive: ") ;

            result += "leftv " + std::to_string(left_power_) ;
            result += ", rightv " + std::to_string(right_power_) ;
            result += ", left " + std::to_string(left_) ;
            result += ", right " + std::to_string(right_) ;
            result += ", angle " + std::to_string(angle_) ;
            result += ", leftenc " + std::to_string(left_enc_value_) ;
            result += ", rightenc " + std::to_string(right_enc_value_) ;
            result += ", xpos " + std::to_string(xpos_ ) ;
            result += ", ypos " + std::to_string(ypos_ ) ;

            return result ;
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
            //
            // Calculate the new desired revolutions per second (RPS)
            //
            double desired_left_rps = left_power_ * left_rps_per_power_per_time_  * left_motor_mult_ ;
            double desired_right_rps = right_power_ * right_rps_per_power_per_time_  * right_motor_mult_ ;

#ifdef TANKDRIVE_PRINT_STUFF
            std::cout << "--------------------------------------------------------" << std::endl ;
            std::cout << "Power " << left_power_ << " " << right_power_ << std::endl ;
            std::cout << "Desired RPS " << desired_left_rps << " " << desired_right_rps << std::endl ;
#endif

            //
            // Now, see how much our RPS can actually changes
            //
            current_left_rps_ = capValue(current_left_rps_, desired_left_rps, current_max_change_) ;
            current_right_rps_ = capValue(current_right_rps_, desired_right_rps, current_max_change_) ;

#ifdef TANKDRIVE_PRINT_STUFF
            std::cout << "Current RPS " << current_left_rps_ << " " << current_right_rps_ << std::endl ;
#endif

            //
            // Now, calculate distance each wheel moved based on the actual RPS
            //
            double dleft = current_left_rps_* dt * diameter_ * PI;
            double dright = current_right_rps_ * dt * diameter_ * PI;

#ifdef TANKDRIVE_PRINT_STUFF
            std::cout << "Current Distance " << dleft << " " << dright << std::endl ;
#endif

            //
            // And add to the total distance so far
            //
            left_ += dleft;
            right_ += dright;

            double lrevs = left_ / (PI * diameter_);
            double rrevs = right_ / (PI * diameter_) ;

            double dv = (dright - dleft) / 2 * scrub_;
            angle_ = normalizeAngleRadians(angle_ + (dv * 2.0) / width_) ;
            updatePosition(dleft, dright, angle_) ;

            double dist = std::sqrt((xpos_ - last_xpos_) * (xpos_ - last_xpos_) + (ypos_ - last_ypos_) * (ypos_ - last_ypos_)) ;
            speed_ = dist / dt ;

            last_xpos_ = xpos_ ;
            last_ypos_ = ypos_ ;

            left_enc_value_ = static_cast<int32_t>(lrevs * ticks_per_rev_ * left_encoder_mult_) ;
            right_enc_value_ = static_cast<int32_t>(rrevs * ticks_per_rev_ * right_encoder_mult_) ;

            if (left_enc_ != nullptr)
                left_enc_->SimulatorSetValue(left_enc_value_) ;

            if (right_enc_ != nullptr)
                right_enc_->SimulatorSetValue(right_enc_value_) ;

            if (left_spark_encoder_ != nullptr)
                left_spark_encoder_->SimulatorSetPosition(left_enc_value_);

            if (right_spark_encoder_ != nullptr)
                right_spark_encoder_->SimulatorSetPosition(right_enc_value_);

            if (navx_ != nullptr) {
                double deg = normalizeAngleDegrees(-rad2deg(angle_ + navx_offset_)) ;
                navx_->SimulatorSetYaw(deg) ;
                navx_->SimulatorSetTotalAngle(-rad2deg(total_angle_ + navx_offset_));
            }

#ifdef NOTYET
            MessageLogger &logger = getRobotMessageLogger() ;
            logger.startMessage(MessageLogger::MessageType::info) ;
            logger << "TankDriveModel:" ;
            logger << " x " << xpos_ ;
            logger << " y " << ypos_ ;
            logger << " angle " << angle_ ;
            logger << " ldist " << left_ ;
            logger << " rdist " << right_ ;
            logger.endMessage()  ;
#endif
        }

        void TankDriveModel::inputChanged(SimulatedObject *obj) {
            std::lock_guard<std::mutex> lock(getLockMutex()) ;

            TalonSRX *talon = dynamic_cast<TalonSRX *>(obj) ;
            CANSparkMax *spark = dynamic_cast<CANSparkMax *>(obj) ;
            if (mt_ == MotorType::MTTalon && talon != nullptr) {
                auto itl = std::find(std::begin(left_talon_motors_), std::end(left_talon_motors_), talon) ;
                if (itl != std::end(left_talon_motors_)) {
                    left_power_ = talon->Get() ;
                }

                auto itr = std::find(std::begin(right_talon_motors_), std::end(right_talon_motors_), talon) ;
                if (itr != std::end(right_talon_motors_)) {
                    right_power_ = talon->Get() ;
                }
            }
            else if (mt_ == MotorType::MTSpark && spark != nullptr)
            {
                auto itl = std::find(std::begin(left_spark_motors_), std::end(left_spark_motors_), spark) ;
                if (itl != std::end(left_spark_motors_)) {
                    left_power_ = spark->SimulatorGetPower() ;
                }

                auto itr = std::find(std::begin(right_spark_motors_), std::end(right_spark_motors_), spark) ;
                if (itr != std::end(right_spark_motors_)) {
                    right_power_ = spark->SimulatorGetPower() ;
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
                if (sol->Get() && gear_ != Gear::LowGear)
                    lowGear() ;
                else if (!sol->Get() && gear_ != Gear::HighGear)
                    highGear() ;
            }
        }

        void TankDriveModel::addDevice(TalonSRX *motor) {
            auto it = std::find(left_motors_.begin(), left_motors_.end(), motor->GetDeviceID()) ;
            if (it != left_motors_.end())
            {
                left_talon_motors_.push_back(motor) ;
                motor->addModel(this) ;
                return ;
            }

            it = std::find(right_motors_.begin(), right_motors_.end(), motor->GetDeviceID()) ;
            if (it != right_motors_.end())
            {
                right_talon_motors_.push_back(motor) ;
                motor->addModel(this) ;
                return ;
            }            
        }

        void TankDriveModel::addDevice(CANSparkMax *motor) {
            auto it = std::find(left_motors_.begin(), left_motors_.end(), motor->GetDeviceID()) ;
            if (it != left_motors_.end())
            {
                left_spark_motors_.push_back(motor) ;
                motor->addModel(this) ;
                return ;
            }

            it = std::find(right_motors_.begin(), right_motors_.end(), motor->GetDeviceID()) ;
            if (it != right_motors_.end())
            {
                right_spark_motors_.push_back(motor) ;
                motor->addModel(this) ;
                return ;
            }  
        }        

        void TankDriveModel::addDevice(CANEncoder *encoder) {
            auto itl = std::find_if(left_spark_motors_.begin(), left_spark_motors_.end(), [encoder](rev::CANSparkMax *motor) { return encoder->SimulatorGetMotor() == motor; });
            if (itl != left_spark_motors_.end())
                left_spark_encoder_ = encoder ;

            auto itr = std::find_if(left_spark_motors_.begin(), left_spark_motors_.end(), [encoder](rev::CANSparkMax *motor) { return encoder->SimulatorGetMotor() == motor; });
            if (itr != left_spark_motors_.end())
                right_spark_encoder_ = encoder ;
        }  

        void TankDriveModel::addDevice(Encoder *encoder) {
            int first, second ;
            encoder->SimulatorGetDigitalIOs(first, second) ;
            if (first == left_encoder_1_ && second == left_encoder_2_) {
                left_enc_ = encoder ;
                left_enc_->addModel(this) ;
            }
            else if (first == left_encoder_2_ && second == left_encoder_1_) {
                left_enc_ = encoder ;
                left_enc_->addModel(this) ;
                left_encoder_mult_ *= -1 ;
            }
            else if (first == right_encoder_1_ && second == right_encoder_2_) {
                right_enc_ = encoder ;
                right_enc_->addModel(this) ;
            }
            else if (first == right_encoder_2_ && second == right_encoder_1_) {
                right_enc_ = encoder ;
                right_enc_->addModel(this) ;
                right_encoder_mult_ *= -1 ;
            }            
        }

        void TankDriveModel::addDevice(AHRS *navx) {
            navx_ = navx ;
            navx_->addModel(this) ;
        }

        void TankDriveModel::addDevice(Solenoid *sol) {
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

            double dangle = xero::math::normalizeAngleRadians(angle_ - last_angle_);
            total_angle_ += dangle;
        }
    }
}
