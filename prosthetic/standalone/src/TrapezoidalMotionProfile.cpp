#include "TrapezoidalMotionProfile.h"
#include <cmath>
#include <fmt/format.h>

TrapezoidalMotionProfile::State TrapezoidalMotionProfile::Calculate(
    const double period, State measuredState, State goalState) const {
  // TODO refactor this stuff into a motion profile generator
  int m_direction = 1;

  if (std::abs(measuredState.position - goalState.position) < 0.05) {
    return goalState;
  }
  if (measuredState.position > goalState.position) {
    m_direction = -1;
  }
  return {
      measuredState.position + (constraints.maxVelocity * period * m_direction),
      0};

  // // Determine if the profile is flipped
  // if (measuredState.position > goalState.position) {
  //   measuredState = measuredState.UnaryMinus();
  //   goalState = goalState.UnaryMinus();
  // }
  //
  // // Assume the current velocity is legal to avoid errors
  // if (measuredState.velocity > constraints.maxVelocity) {
  //   measuredState.velocity = constraints.maxVelocity;
  // }
  //
  // // Handle truncated motion profile where V0 or Vf is non-zero. Do this by
  // // solving predicted initial and final parameters. T = V/A Distance =
  // 1/2at^2 const double truncatedProfileStartTime =
  //     measuredState.velocity / constraints.maxAcceleration;
  // const double truncatedProfileStartDistance =
  //     (constraints.maxAcceleration / 2.0) *
  //     (truncatedProfileStartTime * truncatedProfileStartTime);
  // const double truncatedProfileEndTime =
  //     goalState.velocity / constraints.maxAcceleration;
  // const double truncatedProfileEndDistance =
  //     (constraints.maxAcceleration / 2.0) *
  //     (truncatedProfileEndTime * truncatedProfileEndTime);
  //
  // // Solve the remaining parameters
  // const double fullDistance = truncatedProfileStartDistance +
  //                             (goalState.position - measuredState.position) +
  //                             truncatedProfileEndDistance;
  // double maxAccelerationTime =
  //     constraints.maxVelocity / constraints.maxAcceleration;
  // double fullSpeedDistance = fullDistance - maxAccelerationTime *
  //                                               maxAccelerationTime *
  //                                               constraints.maxAcceleration;
  //
  // // Handle the case where the profile never reaches full speed
  // if (fullSpeedDistance < 0) {
  //   maxAccelerationTime = std::sqrt(fullDistance /
  //   constraints.maxAcceleration); fullSpeedDistance = 0;
  // }
  //
  // // Calculate the times of the vertices of the profile (start accel, end
  // accel,
  // // end speed, end Deccel)
  // const double endAccelerationTime =
  //     maxAccelerationTime - truncatedProfileStartTime;
  // const double endFullSpeedTime =
  //     endAccelerationTime + fullSpeedDistance / constraints.maxVelocity;
  // const double endDeccelTime =
  //     endFullSpeedTime + maxAccelerationTime - truncatedProfileEndTime;
  //
  // // Based on the current time in the profile in the profile, apply the
  // speeds
  // // to the current state to form the setpoint state.
  // State setpoint{measuredState.position, measuredState.velocity};
  //
  // // Use kinematics to determine setpoint position and velocity
  // if (period < endAccelerationTime) {
  //   setpoint.velocity += period * constraints.maxAcceleration;
  //   setpoint.position +=
  //       (measuredState.velocity + period * constraints.maxAcceleration / 2.0)
  //       * period;
  // } else if (period < endFullSpeedTime) {
  //   setpoint.velocity = constraints.maxVelocity;
  //   setpoint.position +=
  //       (measuredState.velocity +
  //        endAccelerationTime * constraints.maxAcceleration / 2.0) *
  //           endAccelerationTime +
  //       constraints.maxVelocity * (period - endAccelerationTime);
  // } else if (period <= endDeccelTime) {
  //   setpoint.velocity = goalState.velocity +
  //                       (endDeccelTime - period) *
  //                       constraints.maxAcceleration;
  //   const double timeLeft = endDeccelTime - period;
  //   setpoint.position =
  //       goalState.position -
  //       (goalState.velocity + timeLeft * constraints.maxAcceleration / 2.0) *
  //           timeLeft;
  // } else {
  //   // We have already elapsed all states of the profile, the setpoint should
  //   // just be the goal.
  //   setpoint = goalState;
  // }
  //
  // if(measuredState.position > goalState.position) {
  //   return setpoint.UnaryMinus();
  // }
  //
  // return setpoint.UnaryMinus();
}
