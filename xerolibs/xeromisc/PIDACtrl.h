#pragma once
#include "SettingsParser.h"

/// \file

namespace xero {
    namespace misc{

        /// \brief a modified drivebase follower used to follow motion profiles
        /// This follower is intended to follow a motion profile that provides the
        /// desired position, velocity and acceleration.
        class PIDACtrl{
        public:

            /// \brief Create the follower with constants from the settings file
            /// \param parser the settings parser
            /// \param kv the name of the kv constant in the settings file
            /// \param ka the name of the ka constant in the settings file
            /// \param kp the name of the kp constant in the settings file
            /// \param kd the name of the kd constant in the settings file
            /// \param angle if true, this controller is managing an angle that is between -180 and 180 degrees            
            PIDACtrl(SettingsParser &parser, const std::string &kv, const std::string &ka, const std::string &kp, const std::string &kd, bool angle = false);

            /// \brief Create the follower with constants from the settings file
            /// This constructure is used if the feed forward equation for velocity is of the form Y = MX + B.
            /// \param parser the settings parser
            /// \param kv1 the name of the kv1 constant in the settings file
            /// \param kv2 the name of the kv2 constant in the settings file            
            /// \param ka the name of the ka constant in the settings file
            /// \param kp the name of the kp constant in the settings file
            /// \param kd the name of the kd constant in the settings file
            /// \param angle if true, this controller is managing an angle that is between -180 and 180 degrees            
            PIDACtrl(SettingsParser &parser, const std::string &kv1, const std::string &kv2, const std::string &ka, const std::string &kp, const std::string &kd, bool angle = false);

            /// \brief Create the follower with constants provided
            /// \param kv the kv constant
            /// \param ka the ka constant
            /// \param kp the kp constant
            /// \param kd the kd constant
            /// \param angle if true, this controller is managing an angle that is between -180 and 180 degrees
            PIDACtrl(double kv, double ka, double kp, double kd, bool angle = false);

            /// \brief returns the output given the follower values
            /// \param a the desired acceleration
            /// \param v the desired velocity
            /// \param dtarget the target distance
            /// \param dactual the actual distance
            /// \param dt the delta time since the last time this was called
            /// \returns the output value for the controller
            double getOutput(double a, double v, double dtarget, double dactual, double dt);

            /// \brief returns the V portion of the output value
            /// \returns the V portion of the output value
            double getVPart() {
                return vpart_ ;
            }

            /// \brief returns the A portion of the output value
            /// \returns the A portion of the output value
            double getAPart() {
                return apart_ ;
            }

            /// \brief returns the P portion of the output value
            /// \returns the P portion of the output value
            double getPPart() {
                return ppart_ ;
            }

            /// \brief returns the D portion of the output value
            /// \returns the D portion of the output value
            double getDPart() {
                return dpart_ ;
            }

            /// \brief returns the last error value
            /// \returns the last error value
            double getLastError() {
                return last_error_ ;
            }

        private:
            double kv_;
            double kv1_ ;
            double kv2_ ;
            double ka_;
            double kp_;
            double kd_;
            double last_error_;
            double angle_ ;

            bool equ_ ;

            double vpart_ ;
            double apart_ ;
            double ppart_ ;
            double dpart_ ;
        };
    }
}