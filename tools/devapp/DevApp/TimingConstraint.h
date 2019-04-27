#pragma once

#include "MinMaxAcceleration.h"
#include "Pose2dWithCurvature.h"
#include "ConstrainedPoint.h"

namespace xero
{
	namespace paths
	{
		class TimingConstraint
		{
		public:
			TimingConstraint() {
			}

			virtual ~TimingConstraint() {
			}

			virtual double getMaxVelocity(const ConstrainedPoint& state)  = 0;
			virtual MinMaxAcceleration getMinMacAccel(const ConstrainedPoint& state, double velocity) = 0;
		};
	}
}

