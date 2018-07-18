#pragma once

#include "Subsystem.h"
#include <AHRS.h>

namespace xero {
	namespace base {
		/// \brief the drivebase for the robot
		class Drivebase : public Subsystem {
			/// \brief This is a directive for the drivebase.  All directives for the drivebase should be derived form this class.
			class DrivebaseDirective : public Action {
			} ;

			/// \brief This is directive meant for the drivebase to drive straight a given distance
			class DriveStraightDirective : public DrivebaseDirective {
			public:
				/// \brief create a new DriveStraightDirective for the distance given
				/// \param target_distance the distance to drive
				DriveStraightDirective(double target_distance);

				/// \brief start the directive, called once per directive
				virtual void start() ;

				/// \brief manages the directive, called each time through the robot loop
				virtual void run() ;

				/// \brief called to determine if the directive is complete
				virtual bool isDone() const ;

				/// \brief called when the directive is complete
				// QUESTION: who calls this method ????
				virtual void end() ;

			private:
				double target_distance_;
			} ;

		public:
			/// \brief create a new drivebase object
			/// \param robot the robot that contains this drivebase subsystem
			Drivebase(Robot& robot);
			
			/// \brief return the current angle of the robot relative to its starting angle
			/// \returns the current angle of the robot
			double getAngle() const {
				return angle_ ;
			}

			/// \brief compute the current state of the robot.
			/// This method generally reads the input sensors associated with the drivebase and
			/// calculates the current state of the drivebase, namely the angle of the robot.
			/// This method is generally called once per robot loop before any robot controller gets a chance
			/// to run.
			virtual void computeState();

			/// \brief run the drivebase subsystem
			/// This method is generally called after the robot controller gets a chance to assign
			/// directives to subsystems.  This method looks at any assigned subsytsem and the current
			/// state of the robot and decides what outputs to set to achieve the desired directive.  If
			/// no directive is assigned, the robot stops and remains still.
			// QUESTION: Is there not a better name than run() ????
			virtual void run() {
			}

			/// \brief set the directive for the drivebase
			virtual bool setDirective(std::shared_ptr<Action> directive) ;

		private:
			AHRS navx_;

			double angle_;
		} ;
	}
}
