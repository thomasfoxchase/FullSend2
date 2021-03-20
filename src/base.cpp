#include "main.h"
#include "base.h"
#include "definitions.h"
#include "motors.h"
#include "telemetry.h"

pros::Mutex base_mutex;

void setBaseCurrent(int current) {
  right_front_mtr.set_current_limit(current);
	left_front_mtr.set_current_limit(current);
  right_middle_mtr.set_current_limit(current);
	left_middle_mtr.set_current_limit(current);
	right_back_mtr.set_current_limit(current);
	left_back_mtr.set_current_limit(current);
}

void baseControl(void* param) { //tank control
  //manage base
  std::uint32_t now = pros::millis();
  setBaseCurrent(DEFAULT_CURRENT);

  while (true) {
    if (base_mutex.take(MUTEX_WAIT_SHORT)) {
      int left = master.get_analog(ANALOG_LEFT_Y); //left analog stick
      int right = master.get_analog(ANALOG_RIGHT_Y); //right analog stick

      left_back_mtr = left;
      right_back_mtr = right;
      left_front_mtr = left;
      right_front_mtr = right;
      left_middle_mtr = left;
      right_middle_mtr = right;

			base_mutex.give();
    }
    pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
  }
}

void baseControlTaskInit() {
  pros::Task base_task(baseControl,(void*)"BASE_TASK");
}

// void basePDMove() {
//
// }
//
//
// void baseMovePID(double distance) {
//
//     //ENDPOINT defines the final value which the encoders should
//     //report after it moves the desired distance
//     const double ENDPOINT = gyroGetAbsolute() + distance;
//
//     double currentValue = gyroGetAbsolute();
//     double currentError = ENDPOINT - currentValue;
//     double previousError = 0.00;
//     double totalError = 0.00;
//     const double INTEGRAL_LIMIT = 10.0;
// 		double kP = 0.500; //0.50 //KU = 4.00 //TU = 0.93
// 		double kI = 0.030; //0.03
// 		double kD = 0.740; //0.74
//
//     //While loop ensures that the robot will keep
//     //turning until it reaches the endpoint
//     while(fabs(currentError) > 0.50) {
//
//         if(fabs(currentError) < INTEGRAL_LIMIT)
//             totalError += currentError;
//         else
//             totalError = 0;
//
//         if(previousError * currentError <= 0) totalError = 0;
//
//         double p = kP * currentError;
//         double i = kI * totalError;
//         double d = kD * (currentError - previousError);
//
//       baseTurn(p+i+d);
//
//         currentValue = gyroGetAbsolute();
//         previousError = currentError;
//         currentError = ENDPOINT - currentValue;
//
//         pros::delay(20);
//     }
//
//     //Once turning is complete, stop all motors by
//     //braking to prevent overturning
// 	  brakeStop();
// }
//
//
// void gyroPIDTurnRelative(double angle) {
//
//     //ENDPOINT defines the final value which the gyro should
//     //report after it turns through the desired angle
//     const double ENDPOINT = gyroGetAbsolute() + angle;
//
//     double currentValue = gyroGetAbsolute();
//     double currentError = ENDPOINT - currentValue;
//     double previousError = 0.00;
//     double totalError = 0.00;
//     const double INTEGRAL_LIMIT = 10.0;
// 		double kP = 0.500; //0.50 //KU = 4.00 //TU = 0.93
// 		double kI = 0.030; //0.03
// 		double kD = 0.740; //0.74
//
//     //While loop ensures that the robot will keep
//     //turning until it reaches the endpoint
//     while(fabs(currentError) > 0.50) {
//
//         if(fabs(currentError) < INTEGRAL_LIMIT)
//             totalError += currentError;
//         else
//             totalError = 0;
//
//         if(previousError * currentError <= 0) totalError = 0;
//
//         double p = kP * currentError;
//         double i = kI * totalError;
//         double d = kD * (currentError - previousError);
//
//       baseTurn(p+i+d);
//
//         currentValue = gyroGetAbsolute();
//         previousError = currentError;
//         currentError = ENDPOINT - currentValue;
//
//         pros::delay(20);
//     }
//
//     //Once turning is complete, stop all motors by
//     //braking to prevent overturning
// 	  brakeStop();
// }
//
//
//
// // Direction Correction
// // Use the gyro and a desired heading to correct steering errors
// void baseMovePHControl(double steeringScale, int minPower, int maxPower, double feet, int timeout, double heading, double setTolerance, double correctionPower, int current_limit) {
// 	setBaseCurrent(AUTON_CURRENT);
// 	std::uint32_t start_time = pros::millis();
// 	std::uint32_t elapsed_time = 0;
// 	double feetPowerCutDistanceToTarget = 1;
//   int power, leftPower, rightPower;
//   double percentP;
// 	// int minPower = 20;
//   double minPercentP = minPower/abs(maxPower);
//   double steeringError = 0.0;
// 	right_front_mtr.tare_position();
// 	left_front_mtr.tare_position();
// 	right_back_mtr.tare_position();
// 	left_back_mtr.tare_position();
//   double feetActual = 0.0;
//   double error = feet; //arbitrary preset
//   if (base_mutex.take(MUTEX_WAIT_SHORT)) {
// 	while (fabs(feetActual) < fabs(feet) && elapsed_time < timeout && error > setTolerance) {
//     percentP = (fabs(feet) - fabs(feetActual))/fabs(feetPowerCutDistanceToTarget);
//     if (percentP < 1) {
// 			power = maxPower * std::max(percentP, minPercentP);
// 		} else {
// 			power = maxPower;
// 		}
//     // Add steering corrections
//     steeringError = heading - gyroGetAbsolute();
//     leftPower = power - steeringScale * steeringError - correctionPower;
//     rightPower = power + steeringScale * steeringError + correctionPower;
//
// 		right_front_mtr = rightPower;
// 		left_front_mtr = leftPower;
// 		right_back_mtr = rightPower;
// 		left_back_mtr = leftPower;
// 		pros::delay(10);
//     feetActual = (right_back_mtr.get_position() + left_back_mtr.get_position())/(825*2); //ticks to feet scale factor
//     error = fabs(feet) - fabs(feetActual);
//     elapsed_time = pros::millis() - start_time;
//   }
//   base_mutex.give(); //do i need a brake?
//   }
// }
