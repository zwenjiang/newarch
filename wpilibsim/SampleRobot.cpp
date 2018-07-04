#include "SampleRobot.h"
#include "RobotSimulator.h"
#include <stdexcept>


namespace frc
{
	SampleRobot::SampleRobot()
	{
		m_enabled = false;
		m_mode = RobotMode::Autonomous;
		m_robotMainOverridden = false;
		m_start_delay = 0.25;
		m_auto_period = 30;
		m_teleop_period = 0;
		m_print_state = false ;
	}

	SampleRobot::~SampleRobot()
	{
		m_controller.join();
	}

	void SampleRobot::robotSetup(SampleRobot *robot_p) {
		try {
			std::cout << "\r\n********** Robot program starting **********\r\n" << std::endl;
			robot_p->StartCompetition();
		}
		catch (const std::exception &ex)
		{
			std::cout << "\r\n******* Robot threw exception - " << ex.what() << " **************\r\n" << std::endl;
		}
	}

	void SampleRobot::InternalControl()
	{
		int ms;
		m_auto_done = false;

		if (m_print_state)
			std::cout << "Set Robot Mode Disabled" << std::endl ;

		setEnabled(false);
		setRobotMode(SampleRobot::RobotMode::Autonomous);
		ms = static_cast<int>(m_start_delay * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));

		if (m_print_state)
			std::cout << "Set Robot Mode Autonomous" << std::endl ;
		setEnabled(true);
		ms = static_cast<int>(m_auto_period * 1000);
		while (ms > 0 && !m_auto_done)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			ms -= 100;
		}

		if (m_print_state)
			std::cout << "Set Robot Mode Disabled (before Teleop)" << std::endl ;

		setEnabled(false);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		if (m_print_state)
			std::cout << "Set Robot Mode Teleop" << std::endl ;

		setRobotMode(SampleRobot::RobotMode::Operator);
		setEnabled(true);

		ms = static_cast<int>(m_teleop_period * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));

		if (m_print_state)
			std::cout << "Set Robot Mode Disabled" << std::endl ;

		setRobotMode(SampleRobot::RobotMode::Finished) ;
		setEnabled(true);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		if (m_print_state)
			std::cout << "Set Robot Mode Finished" << std::endl ;

		m_running = false;
	}

	bool SampleRobot::ParseDoubleArg(size_t index, double &value, const char *flag_p)
	{
		size_t end ;

		if (index == m_args.size()) {
			std::cout << "SampleRobot: argument '" << flag_p << "' requires an argument (real number)" << std::endl ;
			return false ;
		}

		try
		{
			value = std::stod(m_args[index], &end);
			if (end != m_args[index].length())
			{
				std::cout << "SampleRobot: invalid value '" << m_args[index];
				std::cout << "' for argument '" << flag_p << "'" << std::endl;
				return false ;
			}
		}
		catch (const std::exception &)
		{
			std::cout << "SampleRobot: invalid value '" << m_args[index];
			std::cout << "' for argument '" << flag_p << "'" << std::endl;
			return false ;
		}

		return true ;
	}

	bool SampleRobot::ProcessCmdLineArgs()
	{
		bool ret = true;
		size_t index = 1;

		while (index < m_args.size())
		{
			if (m_args[index] == "--start")
			{
				index++;
				if (!ParseDoubleArg(index, m_start_delay, "--start"))
				{
					ret = false ;
					break ;
				}
				index++;
			}
			else if (m_args[index] == "--auto")
			{
				index++;
				if (!ParseDoubleArg(index, m_auto_period, "--auto"))
				{
					ret = false ;
					break ;
				}
				index++;
			}	
			else if (m_args[index] == "--oper")
			{
				index++;
				if (!ParseDoubleArg(index, m_teleop_period, "--oper"))
				{
					ret = false ;
					break ;
				}
				index++;
			}		
			else if (m_args[index] == "--simtime")
			{
				double value ;

				index++;
				if (!ParseDoubleArg(index, value, "--simtime"))
				{
					ret = false ;
					break ;
				}
				index++;

				RobotSimulator &sim = RobotSimulator::get() ;
				sim.setTimeInterval(value) ;
			}
			else if (m_args[index] == "--show") {
				m_print_state = true ;
				index++ ;
			}
			else
			{
				std::cout << "SampleRobot: invalid command line argument '";
				std::cout << m_args[index] << "' - robot aborting" << std::endl;
				std::cout << "Valid Arguments" << std::endl;
				std::cout << "  --start NUMBER" << std::endl;
				std::cout << "  --auto NUMBER" << std::endl;
				std::cout << "  --oper NUMBER" << std::endl;
				ret = false;
				break;
			}
		}

		return ret;
	}


	void SampleRobot::StartCompetition()
	{
		//
		// Process command line arguments
		//
		if (!ProcessCmdLineArgs())
			return;

		//
		// Initialize the robot hardware
		//
		RobotInit();

		//
		// Allow a derived class to take over the main loop
		//
		RobotMain();

		//
		// Start the thread that manages the competition
		//
		m_controller = std::thread(&SampleRobot::InternalControl, this);

		//
		// If the main loop was not taken over, supply a main loop where
		// we ask the robot to handle specific modes
		//
		if (!m_robotMainOverridden) {
			m_running = true;
			while (m_running) {
				if (IsDisabled()) {
					Disabled();
					while (IsDisabled() && !IsFinished());
				}
				else if (IsAutonomous()) {
					Autonomous();
					m_auto_done = true;
					while (IsAutonomous() && IsEnabled());
				}
				else if (IsTest()) {
					Test();
					while (IsTest() && IsEnabled());
				}
				else {
					OperatorControl();
					while (IsOperatorControl() && IsEnabled());
				}
			}

			RobotSimulator::stop();
			std::cout << "\r\n ************ Robot program ending ***********" << std::endl;
		}
	}


	bool SampleRobot::IsDisabled() const 
	{
		return !m_enabled;
	}

	bool SampleRobot::IsFinished() const
	{
		return m_mode == RobotMode::Finished;
	}

	bool SampleRobot::IsAutonomous() const 
	{
		return m_mode == RobotMode::Autonomous;
	}

	bool SampleRobot::IsEnabled() const 
	{
		return m_enabled;
	}

	bool SampleRobot::IsTest() const 
	{
		return m_mode == RobotMode::Test;
	}

	bool SampleRobot::IsOperatorControl() const 
	{
		return m_mode == RobotMode::Operator;
	}
}
