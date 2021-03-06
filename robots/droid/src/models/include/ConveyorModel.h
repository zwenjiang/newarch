#pragma once
#include <SimulatedMotor.h>
#include <SimulationModel.h>
#include <mockdata/DriverStationData.h>
#include <memory>
#include <mutex>
#include <array>

namespace xero
{
    namespace models
    {
        class ConveyorModel : public xero::sim2::SimulationModel
        {            
        public:
            ConveyorModel(xero::sim2::SimulatorEngine &engine, const std::string &inst);
            virtual ~ConveyorModel();

            virtual bool create() ;
            virtual void run(uint64_t microdt);
            virtual void processEvent(const std::string &name, const xero::sim2::SimValue &value) ;

        private:
            static constexpr int MaxBalls = 5 ;
            static constexpr double BallDiameter = 7.0 ;

            struct BallStatus
            {
                double position_ ;
                bool present_ ;
                bool drop_ ;
            } ;

        private:
            void insertBallFromIntake(bool drop) ;
            void deleteBallFromIntake() ;
            void deleteBallFromShooter() ;
            int countBalls() ;
            void printBalls() ;

        private:
            std::shared_ptr<xero::sim2::SimulatedMotor> belt_ ;
            std::shared_ptr<xero::sim2::SimulatedMotor> turret_ ;            
            int number_of_balls_ ;
            int sensorA_ ;
            int sensorB_ ;
            int sensorC_ ;
            int sensorD_ ;
            double dist_per_second_per_volt_ ;

            double sensorAPos_ ;
            double sensorBPos_ ;
            double sensorCPos_ ;
            double sensorDPos_ ;

            BallStatus balls_[MaxBalls] ;

            bool printall_ ;
        };
    }
}