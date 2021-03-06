#include <frc/ScreenVisualizer.h>
#include <TankDriveModel.h>
#include <frc/RobotSimBase.h>
#include <xeromath.h>
#include <cassert>
#include <string>
#include <cmath>
#include <algorithm>

using namespace xero::misc ;
using namespace xero::math ;

//
// Connect the screen visualizer to the callback from the window
// size change signal
//
xero::sim::ScreenVisualizer *theOne = nullptr ;

namespace xero {
    namespace sim {

        const char *ScreenVisualizer::rotate_chars_ = "|/-\\|/-\\" ;

        ScreenVisualizer::ScreenVisualizer(RobotSimBase &sim, const std::string &dbname) : Visualizer(sim) {    
            drivebase_model_ = dbname ;
            inited_ = false ;

            // The physical dimensions of the FRC field
            max_x_ = 647.3 ;
            max_y_ = 336.0 ;    

            assert(theOne == nullptr) ;
            theOne = this ;             

            last_info_valid_ = false ;

            robot_window_ = nullptr ;
            oi_window_ = nullptr ;
            field_window_ = nullptr ;
        }

        ScreenVisualizer::~ScreenVisualizer() {
            deinitScreen() ;
            theOne = nullptr ;
        }

        void ScreenVisualizer::windowSizeChanged() {
            doLayout() ;
        }

        void ScreenVisualizer::beginCycle(double t) {
            if (!inited_) {
                initScreen() ;
                doLayout() ;
                inited_ = true ;
            }

            std::string str ;
            subsystem_row_   = 1 ;
            
            wmove (robot_window_, subsystem_row_++, 0) ;
            str = "Time: " ;
            str += std::to_string(t) ;
            waddstr(robot_window_, str.substr(0, RobotWindowWidth).c_str()) ;
            wclrtoeol(robot_window_) ;

            wmove(robot_window_, subsystem_row_++, 0) ;
            str = "Mode: " + getModeString() ;

            waddstr(robot_window_, str.substr(0, RobotWindowWidth).c_str()) ;
            wclrtoeol(robot_window_) ;
        }

        void ScreenVisualizer::endCycle() {
            std::shared_ptr<TankDriveModel> db = std::dynamic_pointer_cast<TankDriveModel>(getSimulator().getModelByName(drivebase_model_)) ;

            int screenx, screeny ;
            int screenwidth, screenlength ;
            int zerox, zeroy ;

            physicalToScreen(db->getXPos(), db->getYPos(), screenx, screeny) ;
            physicalToScreen(0, 0, zerox, zeroy) ;
            physicalToScreen(db->getWidth(), db->getLength(), screenwidth, screenlength) ;

            screenwidth = screenwidth - zerox ;
            screenlength = zeroy - screenlength ;

            screenx -= screenwidth / 2 ;
            screeny -= screenlength / 2 ; 

            plotRobot(screenx, screeny, db->getAngle(), screenwidth, screenlength) ;
            wrefresh(field_window_) ;
            wrefresh(robot_window_) ;
            wrefresh(oi_window_) ;
        }

        void ScreenVisualizer::drawField() {
            int x1, y1, x2, y2 ;
            physicalToScreen(0.0, 0.0, x1, y2) ;
            physicalToScreen(max_x_, max_y_, x2, y1) ;
            x1-- ;
            y1-- ;
            drawScreenRectangle(field_window_, x1, y1, x2-x1, y2-y1) ;
            drawGameSpecificField(field_window_) ;
            wrefresh(field_window_) ;
        }

        void ScreenVisualizer::drawScreenRectangle(WINDOW *win, int x1, int y1, int width, int height) {
            if (width < 0) {
                width = -width ;
                x1 -= width ;
            }

            if (height < 0) {
                height = -height ;
                y1 -= height ;
            }

            for(int row = 0 ; row < height ; row++) {
                mvwaddch(win, row + y1, x1 + width - 1, ACS_VLINE) ;
                mvwaddch(win, row + y1, x1, ACS_VLINE) ;
            }
            for(int col = 0 ; col < width ; col++) {
                mvwaddch(win, y1 + height - 1, col + x1, ACS_HLINE) ;
                mvwaddch(win, y1, col + x1, ACS_HLINE) ;
            }

            mvwaddch(win, y1, x1, ACS_ULCORNER) ;
            mvwaddch(win, y1 + height - 1, x1, ACS_LLCORNER) ;
            mvwaddch(win, y1, x1 + width - 1, ACS_URCORNER) ;
            mvwaddch(win, y1 + height - 1, x1 + width - 1, ACS_LRCORNER) ;
        }

        void ScreenVisualizer::drawFieldRectangle(WINDOW *win, double x1, double y1, double width, double height) {
            int sx1, sy1, sx2, sy2 ;
            physicalToScreen(x1, y1, sx1, sy1) ;
            physicalToScreen(x1 + width, y1 + height, sx2, sy2) ;
            drawScreenRectangle(win, sx1, sy1, sx2 - sx1, sy2 - sy1) ;
        }
        
        void ScreenVisualizer::drawFilledScreenRectangle(WINDOW *win, char ch, int x1, int y1, int width, int height) {
            if (width < 0) {
                width = -width ;
                x1 -= width ;
            }

            if (height < 0) {
                height = -height ;
                y1 -= height ;
            }

            for(int row = 0 ; row < height ; row++) {
                for(int col = 0 ; col < width ; col++ ) {
                    mvwaddch(win, row + y1, col + x1, ch) ;
                }
            }
        }

        void ScreenVisualizer::drawFilledFieldRectangle(WINDOW *win, char ch, double x1, double y1, double width, double height) {
            int sx1, sy1, sx2, sy2 ;
            physicalToScreen(x1, y1, sx1, sy1) ;
            physicalToScreen(x1 + width, y1 + height, sx2, sy2) ;
            drawFilledScreenRectangle(win, ch, sx1, sy1, sx2 - sx1 + 1, sy2 - sy1 + 1) ;
        }       

        void ScreenVisualizer::initScreen() {
            initscr() ;
            noecho() ;
            cbreak() ;
        }

        void ScreenVisualizer::deinitScreen() {
            endwin() ;
            curs_set(1) ;
        }       

        void ScreenVisualizer::doLayout() {
            getmaxyx(stdscr, height_, width_) ;

            int charwidth = width_ - RobotWindowWidth - OIWindowWidth - 2;
            int charheight = height_ - 2;

            double sx = (double)charwidth / max_x_ ;
            double sy = (double)charheight / max_y_ ;

            if (sx < sy)
                scale_ = sx ;
            else
                scale_ = sy ;

            //
            // Create the window for the field
            //
            int rsx, rsy ;
            int field_width, field_height ;
            physicalToScreen(0, 0, rsx, rsy) ;
            physicalToScreen(max_x_, max_y_, field_width, field_height) ;
            field_width = field_width - rsx + 2 ;
            field_height = rsy - field_height + 2 ;
            field_window_ = newwin(field_height, field_width, 0, 0) ;

            //
            // Create the window for the robot information
            //
            robot_window_ = newwin(height_, RobotWindowWidth, 0, field_width + 1) ;

            //
            // Create the window for the OI information
            //
            oi_window_ = newwin(height_ / 2, width_ - field_width - RobotWindowWidth, 0, field_width + RobotWindowWidth + 3) ;
            nodelay(oi_window_, true) ;

            keypad(oi_window_, true) ;
            keypad(field_window_, true) ;           
            keypad(robot_window_, true) ;
            curs_set(0) ;
            clear() ;
            drawField() ;
        }       

        void ScreenVisualizer::physicalToScreen(double px, double py, int &sx, int &sy) {
            sx = static_cast<int>(px * scale_) + 1 ;
            sy = static_cast<int>((max_y_ - py) * scale_) + 1 ;
        }

        int ScreenVisualizer::angleToSide(double angle) {
            int ret = 0 ;

            if (angle > 50.0)
                ret = 9 ;

            if (std::fabs(normalizeAngleDegrees(angle)) <= 45.0)
                ret = 0 ;           // Right
            else if (std::fabs(normalizeAngleDegrees(angle-90.0)) <= 45.0)
                ret = 1 ;           // Top
            else if (std::fabs(normalizeAngleDegrees(angle-180.0)) < 45.0)
                ret = 2 ;           // Left
            else
                ret = 3 ;           // Bottom

            return ret ;
        }

        void ScreenVisualizer::plotRobot(int x, int y, double angle, int width, int length) {
            char ch ;

            if (last_info_valid_ && x == last_x_ && y == last_y_)
                return ;          

            if (last_info_valid_)
                drawFilledScreenRectangle(field_window_, ' ', last_x_, last_y_, width , length ) ;

            ch = getRobotChar() ;

            drawScreenRectangle(field_window_, x, y, width, length) ;
            drawFilledScreenRectangle(field_window_, ch, x + 1, y + 1, width - 2, length - 2) ;

            int side = angleToSide(rad2deg(angle)) ;
            switch(side) {
            case 0:         // Right
                mvwaddch(field_window_, y + length / 2, x + width - 1, ACS_LTEE) ;
                break ;
            case 1:         // Top
                mvwaddch(field_window_, y, x + width /2 , ACS_BTEE) ;
                break ;         
            case 2:         // Left
                mvwaddch(field_window_, y + length / 2, x, ACS_RTEE) ;
                break ;         
            case 3:         // Bottom
                mvwaddch(field_window_, y + length - 1, x + width /2 , ACS_TTEE) ;
                break ;
            }

            last_x_ = x ;
            last_y_ = y ;
            last_angle_ = angle ;
            last_info_valid_ = true ;
        }       

        void ScreenVisualizer::visualizeSubsystem(std::shared_ptr<SubsystemModel> model_p) {
            std::list<std::string> lines ;
            int fieldwidth = getRobotWindowWidth() ;            

            subsystem_row_++ ;

            wmove(getRobotWindow(), subsystem_row_++, 0) ;
            waddstr(getRobotWindow(), model_p->getName().c_str()) ;

            model_p->generateDisplayInformation(lines) ;
            for(std::string line : lines) {
                if (line.length() > static_cast<size_t>(fieldwidth))
                    line = line.substr(0, fieldwidth) ;
                wmove(getRobotWindow(),subsystem_row_++, 0) ;
                waddstr(getRobotWindow(), line.c_str()) ;
                wclrtoeol(getRobotWindow()) ;
            }
        }
    }
}
