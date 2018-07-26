#pragma once

#include "Action.h"
#include <memory>
#include <map>
#include <list>
#include <string>

namespace xero {
	namespace base {
		//
		// Forward declaraion of the robot class.
		//
		class Robot ;
		class Subsystem ;

		typedef std::shared_ptr<Subsystem> SubsystemPtr ;
		
		/// \brief This is the base class for any subsystem in the system.
		class Subsystem {
		public:	
			class ExecuteNamedSequence : public Action {
			public:
				ExecuteNamedSequence(ActionPtr action_p) ;
				virtual void start();
				virtual void run();
				virtual bool isDone() ;
				virtual void end();
				virtual bool cancel() ;
				virtual std::string toString() ;

			private:
				ActionPtr action_ ;
			} ;

		public:
			/// \brief create a new subsystem
			Subsystem(Robot &robot, const std::string &name) ;

			/// \brief destroy a new subsystem
			virtual ~Subsystem() ;

			/// \brief returns the name of the subsystem
			/// \returns the name of the subsystem
			const std::string &getName() const {
				return name_ ;
			}

			/// \brief add a subsystem as a child of the current subsystem
			void addChild(SubsystemPtr child) {
				children_.push_back(child) ;
			}

			/// \brief compute the current state of the robot.
			/// The subsystem generally reads and associated input sensors and
			/// computes a state that is meaningful to users of the subsystem.
     		virtual void computeState() ;

			/// \brief set the current Action for the subsystem
			/// \param Action the new Action for the subsystem
			/// \return true if the Action is accepted, false if not
			virtual bool setAction(ActionPtr Action);

			/// \brief return a constant pointer to the current Action
			/// \returns  a constant pointer to the current Action
			const ActionPtr getAction() const {
				return action_ ;
			}

			/// \brief returns a pointer to the current Action
			/// \returns a pointer to the current Action
			ActionPtr getAction()  {
				return action_ ;
			}

			/// \brief cancel the current action for this subsystem
			/// It also cancels the actions for any children subsystems
			/// \returns true if the action was canceled, false if it could not be
			virtual bool cancelAction() ;

			/// \brief asks a subsystem to execute a named sequence
			/// \param the name of the sequence to execute
			bool executeNamedSequence(const std::string &name) ;

			/// \brief asks a subsystem to create any named sequences
			virtual void createNamedSequences(SubsystemPtr ptr) {				
			}

			/// \brief set output actuators associated with the subsystem
			/// The output actuators are set to achieve the currently active
			/// Action based on the current state of the subsystem.
			virtual void run() ;
			
			/// \brief returns true if the subsystem is done with the current Action
			/// \returns true if the subsystem is done with the current Action
			virtual bool isDone() const {
				return action_->isDone() ;
			}

			/// \brief return the main robot object
			Robot &getRobot() {
				return robot_ ;
			}			

		protected:
			/// \brief add a named sequence to a subsystem
			/// \param name the name of the sequence to define
			/// \param the step to execute for this sequence
			void defineNamedSequence(const std::string &name, ActionPtr sequence_p) {
				sequences_[name] = sequence_p ;
			}

			/// \brief check that a Action is valid for a subsystem
			/// \param Action the Action to check for a subsystem
			virtual bool canAcceptAction(ActionPtr Action) {
				return false ;
			}

		private:
			//
			// A reference to the robot object that contains this subsystem
			//
			Robot& robot_;

			//
			// The name of the subsystem (e.g. TankDrive)
			//
			std::string name_ ;

			//
			// The currently active Action
			//
			ActionPtr action_;

			//
			// A set of named sequences associatd with this subsystem
			//
			std::map<std::string, ActionPtr> sequences_ ;

			//
			// The set of child subsystems
			//
			std::list<SubsystemPtr> children_ ;
		} ;

		typedef std::shared_ptr<Subsystem> SubsystemPtr ;
	}
}
