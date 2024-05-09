#pragma once

class TrapezoidalMotionProfile {
 public:
  struct Constraints {
    double maxVelocity;
    double maxAcceleration;
  };

  struct State {
    double position;
    double velocity;

    [[nodiscard]]
    State UnaryMinus() const {
      return {-position, -velocity};
    }
  };

  explicit TrapezoidalMotionProfile(const Constraints constraints)
      : constraints(constraints) {}

  [[nodiscard]]
  State Calculate(double period, State measuredState, State goalState) const;

 private:
  Constraints constraints;
};
