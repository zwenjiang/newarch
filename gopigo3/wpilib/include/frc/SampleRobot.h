#pragma once

#include "RobotBase.h"
#include <thread>

namespace frc
{
    class SampleRobot : public RobotBase
    {
    public:
        SampleRobot();
        virtual ~SampleRobot();

        static SampleRobot &GetInstance() {
            return *theone;
        }

        virtual void StartCompetition();

        virtual void RobotInit();
        virtual void Disabled();
        virtual void Autonomous();
        virtual void OperatorControl();
        virtual void Test();
        virtual void RobotMain();

        static void ControlCHandler(int v);

      private:
        void InternalControl();
        void DriverStationControl();
        bool ProcessCmdLineArgs();

    private:
        static SampleRobot *theone ;

        bool m_robotMainOverridden;
        std::thread m_controller;

        bool m_driver_station_run;
        bool m_running;

        // If true, we wait for a driver station to connect
        bool m_station;

        //
        // If station is not try, this is the timing for the robot
        // modes.
        //

        // The delay before we start autonomous
        double m_start_delay;

        // The length of the auto mode in seconds
        double m_auto_period;

        // The length of the teleop mode in seconds
        double m_teleop_period;
    };
}

