#include "Subsystem.h"
#include "Action.h"
#include "Robot.h"
#include "basegroups.h"
#include <MessageLogger.h>

using namespace xero::misc ;

namespace xero {
    namespace base {

        Subsystem::ExecuteNamedSequence::ExecuteNamedSequence(ActionPtr step_p) {
            action_ = step_p ;
        }

        void Subsystem::ExecuteNamedSequence::start() {
            action_->start() ;
        }

        void Subsystem::ExecuteNamedSequence::run() {
            action_->run() ;
        }

        bool Subsystem::ExecuteNamedSequence::isDone() {
            return action_->isDone() ;
        }

        void Subsystem::ExecuteNamedSequence::cancel() {
            action_->cancel() ;
        }

        std::string Subsystem::ExecuteNamedSequence::toString() {
            return action_->toString() ;
        }

        Subsystem::Subsystem(Robot &robot, const std::string &name) : robot_(robot) , name_(name) {
		}

        Subsystem::~Subsystem() {
        }

        void Subsystem::run() {
            for(auto sub: children_)
                sub->run() ;
            
            if (action_ != nullptr)
                action_->run() ;
        }

        void Subsystem::computeState() {
            for(auto sub: children_)
                sub->computeState() ;            
        }

        bool Subsystem::executeNamedSequence(const std::string &name) {
            auto it = sequences_.find(name) ;
            if (it == sequences_.end()) {
                MessageLogger &logger = getRobot().getMessageLogger() ;
                logger.startMessage(MessageLogger::MessageType::error, MSG_GROUP_SUBSYSTEMS) ;
                logger << "subsystem '" << name_ << "'" ;
                logger << " was requested to execute unknown named sequence '" ;
                logger << name << "'" ;
                logger.endMessage() ;
                return false ;
            }
            
            std::shared_ptr<ExecuteNamedSequence> seqdir_p = std::make_shared<ExecuteNamedSequence>(it->second) ;
            return setAction(seqdir_p) ;
        }

		void Subsystem::cancelAction() {
			if (action_ != nullptr)
				action_->cancel() ;

			action_ = nullptr ;
		}

        bool Subsystem::setAction(ActionPtr action) {

            if (action_ != nullptr && !action_->isDone()) {
                //
                // The current Action is still running, interrupt it
                //
                cancelAction();
            }

            if (action != nullptr && !canAcceptAction(action)) {
                MessageLogger &logger = getRobot().getMessageLogger() ;
                logger.startMessage(MessageLogger::MessageType::error, MSG_GROUP_ACTION_SEQ) ;
                logger << "subsystem '" << getName() << "' rejected action '" << action->toString() << "'" ;
                logger.endMessage() ;
                return false ;
            }

			//
			// And now start the Action
			//
            action_ = action ;
            if (action_ != nullptr)
                action_->start() ;
			return true ;
        }
    }
}
