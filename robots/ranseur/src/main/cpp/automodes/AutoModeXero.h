#pragma once

#include "RanseurAutoModeBase.h"

namespace xero
{
    namespace ranseur 
    {
        class AutoModeXero : public RanseurAutoModeBase
        {
        public:
            AutoModeXero(xero::base::Robot &robot) ;
            virtual ~AutoModeXero() ;
            
        } ;
    }
}

