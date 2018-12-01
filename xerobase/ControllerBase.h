#pragma once

#include <string>

namespace xero {
	namespace base {
		class Robot ;
		
		/// \brief This is the base class for all robot controllers.
		/// See Robot for more information about controllers
		class ControllerBase {
		public:
			/// \brief create a new controller base
			/// \param robot the robot this controller controls
			ControllerBase(Robot &robot) ;

			/// \brief destroy the robot controller
			virtual ~ControllerBase() ;

			/// \brief run the controller, called once per robot loop.
			/// This method is abstract and will be defined by a derived controller
			/// class.
			virtual void run() = 0 ;

			/// \brief return a string indicating the state of the controller. 
			/// This is generally used by the logging functions and is mostly used by the
			/// AutoController class to log which auto mode has been selected and is being run.
			/// \returns a string representing the state of the controller
			virtual std::string getControllerInformation() {
				return std::string("") ;
			}

		protected:
			/// \brief Returns a reference to the robot this robot is controlling
			/// \returns a reference to a robot
			Robot &getRobot() {
				return robot_ ;
			}

		private:
			Robot &robot_ ;
		} ;
	}
}
