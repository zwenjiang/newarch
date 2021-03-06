#include <CTREManager.h>
#include <SimulatorMessages.h>

namespace xero
{
    namespace sim2
    {
        CTREManager::CTREManager(SimulatorEngine &engine) : HardwareManager(engine)
        {            
        }

        CTREManager::~CTREManager()
        {

        }

        bool CTREManager::create(int index)
        {
            std::lock_guard<std::mutex> lock(lock_);
            auto &msg = getEngine().getMessageOutput();

            auto it = status_.find(index);
            if (it != status_.end())
            {
                msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Warning);
                msg << "failed to create duplicate CTRE motor controller, index " << index ;
                msg.endMessage(getEngine().getSimulationTime());
                return false;
            }

            CTREStatus st;
            st.power_ = 0.0;
            st.encoder_ = 0.0 ;
            status_[index] = st;

            msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Debug, 5);
            msg << "created CTRE motor controller, index " << index ;
            msg.endMessage(getEngine().getSimulationTime());

            return true;
        }

        bool CTREManager::destroy(int index)
        {
            std::lock_guard<std::mutex> lock(lock_);
            auto &msg = getEngine().getMessageOutput();

            auto it = status_.find(index);
            if (it == status_.end())
            {
                msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Warning);
                msg << "failed to destroy CTRE motor controller - does not exist, index " << index ;
                msg.endMessage(getEngine().getSimulationTime());                
                return false;
            }

            msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Debug, 5);
            msg << "destroyed CTRE motor controller, index " << index ;
            msg.endMessage(getEngine().getSimulationTime());                

            status_.erase(it);
            return true;
        }

        bool flag = false ;
        bool CTREManager::set(int index, double value)
        {
            std::lock_guard<std::mutex> lock(lock_);
            auto &msg = getEngine().getMessageOutput();

            auto it = status_.find(index);
            if (it == status_.end())
            {
                msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Warning);
                msg << "failed to set CTRE motor controller - does not exist, index " << index ;
                msg.endMessage(getEngine().getSimulationTime());                 
                return false;
            }

            if (std::fabs(status_[index].power_ - value) > 1e-3)
            {
                msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Debug, 8);
                msg << "set CTRE motor controller, index " << index << " value " << value ;
                msg.endMessage(getEngine().getSimulationTime());               
            }

            status_[index].power_ = value;
            return true;
        }

        bool CTREManager::get(int index, double &value)
        {
            std::lock_guard<std::mutex> lock(lock_);
            auto &msg = getEngine().getMessageOutput();

            auto it = status_.find(index);
            if (it == status_.end())
            {
                value = 0.0 ;
                return true;
            }

            value = status_[index].power_;

            msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Debug, 8);
            msg << "get CTRE motor controller, index " << index << " value " << value ;
            msg.endMessage(getEngine().getSimulationTime());   

            return true;
        }

        bool CTREManager::getEncoder(int index, double &value)
        {
            std::lock_guard<std::mutex> lock(lock_);
            auto &msg = getEngine().getMessageOutput();

            auto it = status_.find(index);
            if (it == status_.end())
            {
                value = 0.0 ;
                return true;
            }

            value = status_[index].encoder_;

            msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Debug, 8);
            msg << "get CTRE motor controller encoder, index " << index << " value " << value ;
            msg.endMessage(getEngine().getSimulationTime());   

            return true;
        }

        bool CTREManager::setEncoder(int index, double value)
        {
            std::lock_guard<std::mutex> lock(lock_);
            auto &msg = getEngine().getMessageOutput();

            auto it = status_.find(index);
            if (it == status_.end())
            {
                return true;
            }

            if (std::abs(status_[index].encoder_ - value) != 0)
            {
                msg.startMessage(xero::sim2::SimulatorMessages::MessageType::Debug,8);
                msg << "set CTRE motor controller encoder, index " << index << " value " << value ;
                msg.endMessage(getEngine().getSimulationTime());               
            }

            status_[index].encoder_ = value;
            return true;
        }
    }
}