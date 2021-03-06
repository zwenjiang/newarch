#pragma once

#include "Subsystem.h"
#include <networktables/NetworkTable.h>
#include <frc/Relay.h>

namespace xero {
    namespace base {
        class LimeLight : public Subsystem
        {
        public:
            enum class camMode
            {
                VisionProcessor,
                DriverCamera,
                Invalid,
            } ;

            enum class ledMode
            {
                UseLED,
                ForceOff,
                ForceBlink,
                ForceOn,
                Invalid,

            } ;

        public:
            LimeLight(Subsystem *parent) ;
            virtual ~LimeLight() ;

            virtual void computeState() ;
            virtual void run() ;
            virtual bool canAcceptAction(ActionPtr action) ;

            void setCamMode(camMode mode) ;
            void setLedMode(ledMode mode) ;
            void setPipeline(int which) ;
            bool isLimeLightPresent(){
                return present_ ;
            }

            bool isTargetPresent(){
                return tv_ ;
            }

            double getTX(){
                return tx_ ;
            }

            double getTY(){
                return ty_ ;
            }

            double getTA(){
                return ta_ ;
            }

            camMode getCamMode() {
                return cam_mode_ ;
            }

            ledMode getLEDMode() {
                return led_mode_ ;
            }

            double getLatency() {
                //
                // Convert to seconds since everything else is done in seconds
                //
                return total_latency_ / 1000.0 ;
            }

            static std::string toString();

        protected:
            std::shared_ptr<nt::NetworkTable> getNetworkTable() {
                return table_ ;
            }

        private:
            constexpr static const char *camModeName = "camMode" ;
            constexpr static const char *ledModeName = "ledMode" ;
            constexpr static const char *pipelineName = "pipeline" ;
            constexpr static const char *streamName = "stream" ;
            

        private:
            std::shared_ptr<nt::NetworkTable> table_ ;
            camMode cam_mode_ ;
            ledMode led_mode_ ;
            int pipeline_ ;

            double ta_ ;
            bool tv_ ;
            double tx_ ; 
            double ty_ ;
            double camera_latency_ ;
            double network_latency_ ;
            double total_latency_ ;
            bool present_ ;
        } ;
    }
}
