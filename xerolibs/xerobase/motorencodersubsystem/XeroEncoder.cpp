#include "XeroEncoder.h"
#include <xeromath.h>

using namespace xero::misc;

namespace xero {
    namespace base {
        static void invalidEncoder(MessageLogger &logger, const std::string &configName, const std::string &errorMessage) {
            logger.startMessage(MessageLogger::MessageType::error);
            logger << "Invalid encoder declaration '" << configName << "': " << errorMessage;
            logger.endMessage();
            assert(0);
        }

        XeroEncoder::XeroEncoder(MessageLogger &logger, SettingsParser &parser, const std::string &configName, bool angular) {
            angular_ = angular;
            
            std::string quadName = configName + ":quad";
            std::shared_ptr<frc::Encoder> quad;
            if (parser.isDefined(quadName + ":1") || parser.isDefined(quadName + ":2")) {
                // parse a quadrature encoder
                if (!parser.isDefined(quadName + ":1") || !parser.isDefined(quadName + ":2")) {
                    invalidEncoder(logger, quadName, "quadrature encoder requires both pins");
                }
                quad = std::make_shared<frc::Encoder>(
                    parser.getInteger(quadName + ":1"),
                    parser.getInteger(quadName + ":2")
                );
                
                if (parser.isDefined(quadName + ":m")) quadM_ = parser.getDouble(quadName + ":m");
                else invalidEncoder(logger, quadName, "m parameter is required");

                if (parser.isDefined(quadName + ":b")) quadB_ = parser.getDouble(quadName + ":b");
                else invalidEncoder(logger, quadName, "b parameter is required");
            } else quad = nullptr;

            std::string analogName = configName + ":analog";
            if (parser.isDefined(analogName)) {
                // parse an analog encoder
                analog_ = std::make_shared<frc::AnalogInput>(parser.getInteger(analogName));

                if (parser.isDefined(analogName + ":m")) absM_ = parser.getDouble(analogName + ":m");
                else invalidEncoder(logger, analogName, "m parameter is required");

                if (parser.isDefined(analogName + ":b")) absB_ = parser.getDouble(analogName + ":b");
                else invalidEncoder(logger, analogName, "b parameter is required");
            } else analog_ = nullptr;

            std::string pwmName = configName + ":pwm";
            if (parser.isDefined(pwmName)) {
                // parse an analog encoder
                pwm_ = std::make_shared<frc::Counter>(parser.getInteger(pwmName));
                pwm_->SetSemiPeriodMode(true);

                if (parser.isDefined(pwmName + ":m")) absM_ = parser.getDouble(pwmName + ":m");
                else invalidEncoder(logger, pwmName, "m parameter is required");

                if (parser.isDefined(pwmName + ":b")) absB_ = parser.getDouble(pwmName + ":b");
                else invalidEncoder(logger, pwmName, "b parameter is required");
            } else pwm_ = nullptr;

            // validate configuration
            if (pwm_ && analog_) invalidEncoder(logger, configName, "cannot declare two absolute encoders");
            if (!(quad_ || pwm_ || analog_)) {
                invalidEncoder(logger, configName, "must declare :quad:(1,2), :analog, or :pwm");
            }
        }

        double XeroEncoder::getPosition() {
            if (quad_) {
                double result = quadM_*quad_->Get() + quadB_;
                if (angular_) return xero::math::normalizeAngleDegrees(result);
                else return result;
            } else return getAbsolutePosition();
        }

        double XeroEncoder::getAbsolutePosition() {
            double pos;
            if (analog_) pos = analog_->GetVoltage();
            else if (pwm_) pos = pwm_->GetPeriod();
            else assert(0 == "no absolute encoder found");


            double result = absM_*pos + absB_;

            std::cout << "Analog Voltage " << pos << " " << absM_ << " " << absB_ << " " << result << std::endl ;
            if (angular_) return xero::math::normalizeAngleDegrees(result);
            else return result;
        }

        void XeroEncoder::reset() {
            if (quad_) {
                quad_->Reset();
                calibrate();
            }
        }

        void XeroEncoder::calibrate() {
            if (quad_ && (analog_ || pwm_)) {
                calibrate(getAbsolutePosition()) ;
            }
        }

        void XeroEncoder::calibrate(double pos) {
            quadB_ = pos - quadM_ * getPosition() ;
        }
    }
}