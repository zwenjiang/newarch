#include "ActionSequence.h"

using namespace xero::base;


ActionSequence::ActionSequence() {
	isDone_ = false;
}

void ActionSequence::pushAction(ActionPtr action) {
	actionSequence_.push_back(action);
}

void ActionSequence::start() {
	index_ = -1;
}

bool ActionSequence::performActionForIsDoneStatus(ActionPtr action) {
	action->run();
	return action->isDone();
}
void ActionSequence::startNextAction() {
	index_++;
	if (index_ < actionSequence_.size()) {
		actionSequence_[index_]->start();
	}
};

void ActionSequence::run() {
	while (1) {
		if (index_ == -1 || actionSequence_[index_]->isDone()) {
			startNextAction();
			if (index_ >= actionSequence_.size())
				break;

		};
		actionSequence_[index_]->run();
		if (!actionSequence_[index_]->isDone()) {
			break;
		}
	}
}

bool ActionSequence::isDone() {
	return (index_ >= actionSequence_.size());
}

bool ActionSequence::cancel() {
	isDone_ = true;
	return (actionSequence_[index_])->cancel();
}

std::string ActionSequence::toString() {
	return "";
}

void ActionSequence::end() {

}

void ActionSequence::pushSubActionPair(SubsystemPtr subsystem, ActionPtr action, bool block) {
	auto p = std::make_shared<DispatchAction>(subsystem, action, block);
	pushAction(p);
}