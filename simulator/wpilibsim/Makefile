TOPDIR=../..

SOURCES = \
	cpp/AnalogInput.cpp\
	cpp/Counter.cpp\
	cpp/DigitalInput.cpp\
	cpp/DriverStation.cpp\
	cpp/PowerDistributionPanel.cpp\
	cpp/Encoder.cpp\
	cpp/livewindow/LiveWindow.cpp\
	cpp/livewindow/LiveWindowSendable.cpp\
	cpp/PrintVisualizer.cpp\
	cpp/GraphicalVisualizer.cpp\
	cpp/RobotSimBase.cpp\
	cpp/SimEventManager.cpp\
	cpp/SampleRobot.cpp\
	cpp/TimedRobot.cpp\
	cpp/ScreenVisualizer.cpp\
	cpp/SimulatedObject.cpp\
	cpp/smartdashboard/NamedSendable.cpp\
	cpp/smartdashboard/SendableBase.cpp\
	cpp/smartdashboard/SendableBuilderImpl.cpp\
	cpp/smartdashboard/SendableChooserBase.cpp\
	cpp/smartdashboard/SmartDashboard.cpp\
	cpp/Solenoid.cpp\
	cpp/SubsystemModel.cpp\
	cpp/Timer.cpp\
	cpp/VictorSP.cpp\
	cpp/Relay.cpp

SHUFFLEBOARD = \
	cpp/shuffleboard/ComplexWidget.cpp\
	cpp/shuffleboard/LayoutType.cpp\
	cpp/shuffleboard/RecordingController.cpp\
	cpp/shuffleboard/Shuffleboard.cpp\
	cpp/shuffleboard/ShuffleboardComponentBase.cpp\
	cpp/shuffleboard/ShuffleboardContainer.cpp\
	cpp/shuffleboard/ShuffleboardInstance.cpp\
	cpp/shuffleboard/ShuffleboardLayout.cpp\
	cpp/shuffleboard/ShuffleboardTab.cpp\
	cpp/shuffleboard/ShuffleboardWidget.cpp\
	cpp/shuffleboard/SimpleWidget.cpp\
	cpp/shuffleboard/WidgetType.cpp

TARGET = wpilibsim

NEED_XEROBASE=true
NEED_WPIUTIL=true
NEED_NETWORKTABLES = true
NEED_NAVX = true
NEED_CTRE = true
NEED_REV = true
NEED_XEROMISC=true
NEED_MODELS=true

LOCAL_CFLAGS += \
	-Iinclude\
	-Icpp\
	-DDYNAMIC_CAMERA_SERVER

include $(TOPDIR)/makefiles/buildlib.mk



