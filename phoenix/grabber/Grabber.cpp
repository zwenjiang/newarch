#include "Grabber.h"
#include "GrabberAction.h"
#include <Robot.h>

using namespace xero::base;

namespace xero {
    namespace phoenix {
        Grabber::Grabber(xero::base::Robot & robot) :Subsystem(robot,"grabber"){
            int motor=robot.getSettingsParser().getInteger("hw:grabber:motor");
            int enc1=robot.getSettingsParser().getInteger("hw:grabber:encoder1");
            int enc2=robot.getSettingsParser().getInteger("hw:grabber:encoder2");

            motor_ = std::make_shared<frc::VictorSP>(motor);
            encoder_ = std::make_shared<frc::Encoder>(enc1,enc2);

            min_angle_=robot.getSettingsParser().getDouble("grabber:angle:minimum");
            max_angle_=robot.getSettingsParser().getDouble("grabber:angle:maximum");
            degrees_per_tick_=robot.getSettingsParser().getDouble("grabber:degrees_per_tick");

            calibrated_ = false;
        }

        Grabber::~Grabber(){
        }
		
        void Grabber::computeState(){
            encoder_ticks_ = encoder_->Get();
            if(calibrated_){
                angle_ = encoder_ticks_*degrees_per_tick_;
            }
        }

		bool Grabber::canAcceptAction(ActionPtr action) {
			std::shared_ptr<GrabberAction> act_p = std::dynamic_pointer_cast<GrabberAction>(action) ;
			if (act_p == nullptr)
				return false ;

			// TODO: reject GrabberToAngleAction if not calibrated

			return true ;
		}
    }
}