#include "TankDrive.h"
#include <cassert>
#include <ctre/phoenix/MotorControl/SensorCollection.h>

namespace xero {
	namespace base {

		//////////////////////////////////////////////////////////////////
		//
		// DriveStraightDirective
		//
		//////////////////////////////////////////////////////////////////

		void TankDrive::DriveStraightDirective::start() {
		}

		void TankDrive::DriveStraightDirective::run() {
		}

		void TankDrive::DriveStraightDirective::end() {
		}

		bool TankDrive::DriveStraightDirective::cancel() {
			return true;
		}

		bool TankDrive::DriveStraightDirective::isDone() {
			return true;
		}

		//////////////////////////////////////////////////////////////////
		//
		// Drivebase
		//
		//////////////////////////////////////////////////////////////////

		void initTalonList(std::list<int>& ids, std::list<TalonPtr>& talons) {
			//Assuming first id will be master
			for(int id : ids) {
				talons.push_back(std::make_shared<TalonSRX>(id));
				if(talons.size() > 0)
					talons.back()->Follow(*talons.front());
			}
		}

		TankDrive::TankDrive(Robot& robot, std::list<int> left_motor_ids, std::list<int> right_motor_ids) : Drivebase(robot) {
			//The two sides should always have the same number of motors and at least one motor each
			assert((left_motor_ids.size() == right_motor_ids.size()) && (left_motor_ids.size() > 0));

			initTalonList(left_motor_ids, left_motors_);
			initTalonList(right_motor_ids, right_motors_);
		}

		void TankDrive::invertMotors(std::list<int> left_motor_ids, std::list<int> right_motor_ids) {
			for(int id : left_motor_ids) {
				for(TalonPtr talon : left_motors_) {
					if(talon->GetDeviceID() == id)
						talon->SetInverted(true);
				}
			}
			for(int id : right_motor_ids) {
				for(TalonPtr talon : right_motors_) {
					if(talon->GetDeviceID() == id)
						talon->SetInverted(true);
				}
			}
		}

		void TankDrive::computeState() {
			Drivebase::computeState();

			dist_l_ = left_motors_.front()->GetSensorCollection().GetQuadraturePosition();
			dist_r_ = right_motors_.front()->GetSensorCollection().GetQuadraturePosition();
		}
	}
}
