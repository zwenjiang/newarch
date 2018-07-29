#include "PhoenixScreenVisualizer.h"
#include "SampleRobot.h"
#include "RobotSimBase.h"
#include "xeromath.h"

#include <cassert>
#include <csignal>
#include <memory>

#ifdef XEROSCREEN

static xero::sim::phoenix::PhoenixScreenVisualizer *theOne = nullptr ;

extern "C" 
{
    void handleWindowChangeSignal() ;
}

void handleWindowChangeSignal(int v) {
    theOne->windowSizeChanged() ;
}

namespace xero {
    namespace sim {
        namespace phoenix {
            PhoenixScreenVisualizer::PhoenixScreenVisualizer(RobotSimBase &sim) : Visualizer(sim) {
                assert(theOne == nullptr) ;
                theOne = this ;
                inited_ = false ;

                min_x_ = 0.0 ;
                min_y_ = 0.0 ;
                max_x_ = 650.0 ;
                max_y_ = 320.0 ;

                last_row_ = -1 ;
                last_col_ = -1 ;
            }

            PhoenixScreenVisualizer::~PhoenixScreenVisualizer() {
                deinitScreen() ;
            }

            void PhoenixScreenVisualizer::beginCycle(double time) {
                if (!inited_) {
                    initScreen() ;
                    inited_ = true ;
                    doLayout() ;
                }

                std::string str ;
                int fieldwidth = width_ - right_side_ - 2;   

                move (TimeRow, right_side_) ;
                str = "Time: " ;
                str += std::to_string(time) ;
                printw(str.substr(0, fieldwidth).c_str()) ;

                move(TimeRow + 1, right_side_) ;
                str = "Mode: " ;
                if (getSimulator().getRobot()->IsDisabled())
                    str += "Disabled" ;
                else if (getSimulator().getRobot()->IsAutonomous())
                    str += "Autonomous" ;
                else if (getSimulator().getRobot()->IsOperatorControl())
                    str += "Teleop" ;
                else if (getSimulator().getRobot()->IsTest())
                    str += "Test" ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;
            }

            void PhoenixScreenVisualizer::visualizeSubsystem(std::shared_ptr<SubsystemModel> subsystem_p) {
                if (subsystem_p->getName() == "lifter")
                    displayLifter(std::dynamic_pointer_cast<LifterModel>(subsystem_p)) ;
                else if (subsystem_p->getName() == "intake")
                    displayIntake(std::dynamic_pointer_cast<IntakeModel>(subsystem_p)) ;
                else if (subsystem_p->getName() == "tankdrive")
                    displayTankDrive(std::dynamic_pointer_cast<xero::sim::TankDriveModel>(subsystem_p)) ;
                else if (subsystem_p->getName() == "wings")
                    displayWings(std::dynamic_pointer_cast<WingsModel>(subsystem_p)) ;
                else if (subsystem_p->getName() == "grabber")
                    displayGrabber(std::dynamic_pointer_cast<GrabberModel>(subsystem_p)) ;
                else if (subsystem_p->getName() == "cubesensor")
                    displayCubeSensor(std::dynamic_pointer_cast<CubeSensorModel>(subsystem_p)) ;
            }

            void PhoenixScreenVisualizer::displayLifter(std::shared_ptr<LifterModel> subsystem_p) {
                int fieldwidth = width_ - right_side_ - 2;                
                std::string str ;

                move(LifterRow, right_side_) ;
                printw("Lifter") ;

                move(LifterRow + 1, right_side_) ;
                str = "  Height: " ;
                str += std::to_string(subsystem_p->getHeight()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;

                move(LifterRow + 2, right_side_) ;
                str = "  Motor: " ;
                str += std::to_string(subsystem_p->getMotor()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ; 

                move(LifterRow + 3, right_side_) ;
                str = "  Brake: " ;
                str += (subsystem_p->getBrake() ? "ON" : "OFF") ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;       

                move(LifterRow + 4, right_side_) ;
                str = "  Gear: " ;
                str += (subsystem_p->getHighGear() ? "HIGH" : "LOW") ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;

                move(LifterRow + 5, right_side_) ;
                str = "  Limits: " ;
                str += "Top " ;
                str += (subsystem_p->getTopLimit() ? "ON" : "OFF") ;
                str += ", Bottom " ;
                str += (subsystem_p->getBottomLimit() ? "ON" : "OFF") ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;
            }

            void PhoenixScreenVisualizer::displayIntake(std::shared_ptr<IntakeModel> subsystem_p) {
                std::string str ;
                int fieldwidth = width_ - right_side_ - 2;

                move(IntakeRow, right_side_) ;
                printw("Intake") ;

                move(IntakeRow + 1, right_side_) ;
                str = "  Left: " + std::to_string(subsystem_p->getVoltage1()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;

                move(IntakeRow + 2, right_side_) ;
                str = "  Right: " + std::to_string(subsystem_p->getVoltage2()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;                                
            }

            void PhoenixScreenVisualizer::displayTankDrive(std::shared_ptr<xero::sim::TankDriveModel> subsystem_p) {
                std::string str ;
                int fieldwidth = width_ - right_side_ - 2;

                move(TankDriveRow, right_side_) ;
                printw("Tank Drive") ;

                move(TankDriveRow + 1, right_side_) ;
                str = "  Position: " ;
                str += std::to_string(subsystem_p->getXPos()) + ", " + std::to_string(subsystem_p->getYPos()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;

                move(TankDriveRow + 2, right_side_) ;
                str = "  Angle: " ;
                str += std::to_string(xero::misc::rad2deg(subsystem_p->getAngle())) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;

                move(TankDriveRow + 3, right_side_) ;
                str = "  Speed: " ;
                str += std::to_string(subsystem_p->getSpeed()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;    

                move(TankDriveRow + 4, right_side_) ;
                str = "  Max Speed: " ;
                str += std::to_string(subsystem_p->getMaxSpeed()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;                              

                plotRobot(subsystem_p->getXPos(), subsystem_p->getYPos(), subsystem_p->getAngle()) ;
            }

            void PhoenixScreenVisualizer::displayWings(std::shared_ptr<WingsModel> subsystem_p) {
                std::string str ;
                int fieldwidth = width_ - right_side_ - 2;

                move(WingsRow, right_side_) ;
                printw("Wings") ;

                move(WingsRow + 1, right_side_) ;
                str = "  Deployed: " ;
                str += (subsystem_p->getWings() ? "YES" : "NO") ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;                  
            }

            void PhoenixScreenVisualizer::displayGrabber(std::shared_ptr<GrabberModel> subsystem_p) {
                std::string str ;
                int fieldwidth = width_ - right_side_ - 2;

                move(GrabberRow, right_side_) ;
                printw("Grabber") ;

                move(GrabberRow + 1, right_side_) ;
                str = "  Angle: " + std::to_string(subsystem_p->getAngle()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;

                move(GrabberRow + 2, right_side_) ;
                str = "  Motor: " + std::to_string(subsystem_p->getVoltage()) ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;                               
            }

            void PhoenixScreenVisualizer::displayCubeSensor(std::shared_ptr<CubeSensorModel> subsystem_p) {
                std::string str ;
                int fieldwidth = width_ - right_side_ - 2;

                move(CubeSensorRow, right_side_) ;
                printw("Cube Sensor") ;

                bool cube = subsystem_p->getCubeSensed() ;

                move(CubeSensorRow + 1, right_side_) ;
                str = "  Present: " ;
                if (cube)
                    str += "YES" ;
                else
                    str += "NO" ;
                printw(str.substr(0, fieldwidth).c_str()) ;
                clrtoeol() ;                   
            }

            void PhoenixScreenVisualizer::endCycle() {
                refresh() ;
            }

            void PhoenixScreenVisualizer::windowSizeChanged() {
                doLayout() ;
            }

            void PhoenixScreenVisualizer::doLayout() {
                getmaxyx(stdscr, height_, width_) ;
                right_side_ = width_ - RightSideWidth ;
                erase() ;
                drawField() ;
            }

            void PhoenixScreenVisualizer::drawField() {
                for(int row = 0 ; row < height_ ; row++) {
                    mvaddch(row, right_side_ - 1, '|') ;
                    mvaddch(row, 0, '|') ;
                }

                for(int col = 0 ; col < right_side_ ; col++) {
                    mvaddch(0, col, '-') ;
                    mvaddch(height_ - 1, col, '-') ;
                }
            }

            void PhoenixScreenVisualizer::initScreen() {
                signal(SIGWINCH, handleWindowChangeSignal) ;

                initscr() ;
            }

            void PhoenixScreenVisualizer::deinitScreen() {
                while (getch() == ERR) ;
                endwin() ;
            }

            const char *PhoenixScreenVisualizer::rotate_chars_ = "|/-\\|/-\\" ;

            void PhoenixScreenVisualizer::plotRobot(double x, double y, double angle) {
                int top = 1 ;
                int left = 1 ;
                int bottom = height_ - 2 ;
                int right = width_ = right_side_ - 2 ;

                if (x < min_x_ || x > max_x_ || y < min_y_ || y > max_y_)
                    return ;

                int col = static_cast<int>((x - min_x_) / (max_x_ - min_x_) * (right - left + 1)) + left ;
                int row = bottom - static_cast<int>((y - min_y_) / (max_y_ - min_y_) * (bottom - top + 1)) ;

                if (col == last_col_ && row == last_row_) {
                    same_count_++ ;

                    //
                    // Same location as last time
                    //
                    if (angle != last_angle_) {
                        //
                        // Rotating
                        //
                        rotate_index_ = (rotate_index_ + 1) % 8 ;
                        mvaddch(row, col, rotate_chars_[rotate_index_]) ;
                    }
                    else {                        
                        //
                        // Stationary
                        //
                        if (same_count_ > 10)
                            mvaddch(row, col, stationary) ;                        
                        else
                            mvaddch(row, col, moving) ;
                    }
                
                }
                else {
                    //
                    // New location
                    //
                    same_count_ = 0 ;

                    //
                    // First clear the old location
                    //
                    if (last_row_ != -1 && last_col_ != -1)
                        mvaddch(last_row_, last_col_, ' ') ;

                    //
                    // And put the moving character into the plot
                    //
                    mvaddch(row, col, moving) ;
                }

                last_row_ = row ;
                last_col_ = col ;
                last_angle_ = angle ;
            }
        }
    }
}

#endif