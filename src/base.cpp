#include "main.h"
#include "base.h"
#include "definitions.h"
#include "motors.h"
#include "telemetry.h"


//MISC BASE FUNCTIONS

void setBaseCurrent(int current) {
    right_front_mtr.set_current_limit(current);
    left_front_mtr.set_current_limit(current);
    right_middle_mtr.set_current_limit(current);
    left_middle_mtr.set_current_limit(current);
    right_back_mtr.set_current_limit(current);
    left_back_mtr.set_current_limit(current);
}

void tareBase() {
    right_front_mtr.tare_position();
    left_front_mtr.tare_position();
    right_middle_mtr.tare_position();
    left_middle_mtr.tare_position();
    right_back_mtr.tare_position();
    left_back_mtr.tare_position();
}


//BASE DRIVER CONTROL

pros::Mutex base_mutex;

void baseControl(void* param) { //tank control
  //manage base
  std::uint32_t now = pros::millis();
  setBaseCurrent(DEFAULT_CURRENT);

  while (true) {
    if (base_mutex.take(MUTEX_WAIT_SHORT)) {
      int left = master.get_analog(ANALOG_LEFT_Y); //left analog stick
      int right = master.get_analog(ANALOG_RIGHT_Y); //right analog stick
      int b = master.get_digital(DIGITAL_B);
      int x = master.get_digital(DIGITAL_X);
      int y = master.get_digital(DIGITAL_Y);

//      std::cout << "rotation" << getRotation() << std::endl;

      left_back_mtr = left;
      right_back_mtr = right;
      left_front_mtr = left;
      right_front_mtr = right;
      left_middle_mtr = left;
      right_middle_mtr = right;

      if (b) {
          autoFunctionTester(-45);
      } else if (y) {
          autoFunctionTester(-90);
      } else if (x) {
          autoFunctionTester(-180);
      }

      base_mutex.give();
    }
    pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
  }
}

void baseControlTaskInit() {
  pros::Task base_task(baseControl,(void*)"BASE_TASK");
}


//BASE MOVEMENT FUNCTIONS

void baseMove(double leftPower, double rightPower) { //forward is positive
    left_back_mtr = leftPower;
    right_back_mtr = rightPower;
    left_front_mtr = leftPower;
    right_front_mtr = rightPower;
    left_middle_mtr = leftPower;
    right_middle_mtr = rightPower;
}

void baseTurn(double power) { //left is positive
    left_back_mtr = -power;
    right_back_mtr = power;
    left_front_mtr = -power;
    right_front_mtr = power;
    left_middle_mtr = -power;
    right_middle_mtr = power;
}

void baseTester() {
    std::cout << "begin 10 sec test" << std::endl;
    int startTime = pros::millis();
    int elapsedTime = 0;
    while(elapsedTime < 10000) {
        baseMove(127,127);
        pros::delay(20);
        elapsedTime = pros::millis(); - startTime;
    }
    std::cout << "left_front: " << left_front_mtr.get_position() << std::endl;
    std::cout << "left_middle: " << left_middle_mtr.get_position() << std::endl;
    std::cout << "left_back: " << left_back_mtr.get_position() << std::endl;
    std::cout << "right_front: " << right_front_mtr.get_position() << std::endl;
    std::cout << "right_middle: " << right_middle_mtr.get_position() << std::endl;
    std::cout << "right_back: " << right_back_mtr.get_position() << std::endl;
    baseMove(0,0);
}

//BASE AUTON MOVEMENT FUNCTIONS

 void basePIDTurn(double rotation, int timeOut) {

     int start_time = pros::millis();
     int elapsed_time = 0;
     /*ENDPOINT defines the final value which the inertial
      sensor should report after it moves the desired distance*/

     const double ENDPOINT = getRotation() + rotation;
     double currentValue = getRotation();
     double currentError = ENDPOINT - currentValue;
     double previousError = ENDPOINT - currentValue;
     double totalError = 0.00;
     double initialSlew = 0;
     const double INTEGRAL_LIMIT = 20.0;
     double kP; //0.50 //KU = 4.00 //TU = 0.93
     double kI; //0.03
     double kD; //0.74
     if (fabs(rotation) <= 45) { //small turns
         kP = 1.3; //0.50 //KU = 4.00 //TU = 0.93
         kI = 0.09; //0.03
         kD = 3; //0.74
     } else if (fabs(rotation) > 90) { //large turns
         kP = 1; //0.50 //KU = 4.00 //TU = 0.93
         kI = 0.06; //0.03
         kD = 5; //0.74
     } else { //medium turns
         kP = 1; //0.50 //KU = 4.00 //TU = 0.93
         kI = 0.07; //0.03
         kD = 5; //0.74
     }


     double kIS = 1;

     //While loop ensures that the robot will keep
     //turning until it reaches the endpoint
     while(fabs(currentError) > 0.50 && elapsed_time < timeOut) {

         if(fabs(currentError - ENDPOINT) < 30) {
//            initialSlew = -(1 - (ENDPOINT - currentError)/**(distance*0.5)*/);
             if (fabs(rotation) < 30) {
                 initialSlew = 1.2;
//                 std::cout << "slew rate sml raw: " << 1;
                 std::cout << "endpoint: " << ENDPOINT << "currentError: " << currentError;
//                 initialSlew = std::min((ENDPOINT - currentError)/30 + 0.8, 1.0); //multiply scalar from 0.6 >> 1 for small movements
             } else if (fabs(rotation) > 90) {
                 std::cout << "endpoint: " << ENDPOINT << "currentError: " << currentError;
                 initialSlew = std::min(fabs((ENDPOINT - currentError))/30 + 0.4, 1.0); //multiply scalar from 0.2 >> 1 for large movements
             } else {
//                 std::cout << "slew rate med raw: " << fabs(ENDPOINT - currentError);
                 std::cout << "endpoint: " << ENDPOINT << "currentError: " << currentError;
                 initialSlew = std::min(fabs((ENDPOINT - currentError))/30 + 0.7, 1.0); //multiply scalar from 0.3 >> 1 for medium movements
             }
         } else {
//            initialSlew = 0;
             initialSlew = 1;
             std::cout << " ELSE NAHHHH ";
         }

//         if(fabs(currentError - ENDPOINT) < 20) {
//             initialSlew = -(20 - (ENDPOINT - currentError))/**(rotation*0.2)*/;
//         } else {
//             initialSlew = 0;
//         }

         if(fabs(currentError) < INTEGRAL_LIMIT) {
             totalError += currentError;
         } else {
             totalError = 0;
         }

         if(previousError * currentError <= 0) { //integral is not meant to slow the turn, it should wither do nothing or speed it up
             totalError = 0;
         }

         double p = std::min(kP * currentError, 90.0);
         double i = kI * totalError;
         double d = kD * (currentError - previousError);
         double is = kIS * initialSlew; //proportional initial speed control

         std::cout << "p: " << p;
         std::cout << " i: " << i;
         std::cout << " d: " << d;
         std::cout << " is: " << is <<std::endl;

//         if (p+i+d < 15) {
//             baseTurn(15);
//         } else {
             baseTurn((p + i + d)*is);
//         }
         currentValue = getRotation();
         previousError = currentError;
         currentError = ENDPOINT - currentValue;
         elapsed_time = pros::millis() - start_time;
         pros::delay(20);
     }
     baseTurn(0);
 }

void basePIDMove(double distance, int timeOut) { //distance in feet
    //ENDPOINT defines the final value which the encoders should
    //report after it moves the desired distance
    int start_time = pros::millis();
    int elapsed_time = 0;
    tareBase();
    const double ENDPOINT = distance; //drive motor encoders are tared to 0 so we don't need an offset
    double currentValue = getDriveMotorsEncoderAvg();
    double currentError = ENDPOINT - currentValue;
    double previousError = ENDPOINT - currentValue;
    double totalError = 0.00;
    double steeringError = 0;
    double initialHeading = getRotation();
//    double setHeading = getRotation() + heading;
    double initialSlew = 0;
    const double INTEGRAL_LIMIT = 1.5;
    double kP; //0.50 //KU = 4.00 //TU = 0.93
    double kI; //0.03
    double kD; //0.74
    double kS = 2;
    double kIS = 1; //60
    if (fabs(distance) >= 2) { //large movements
        kP = 30; //0.50 //KU = 4.00 //TU = 0.93
        kI = 0.5; //0.03
        kD = 90; //0.74
    } else  { //tiny movements
        kP = 50; //0.50 //KU = 4.00 //TU = 0.93
        kI = 0.5; //0.03
        kD = 90; //0.74
    }



    //While loop ensures that the robot will keep
    //turning until it reaches the endpoint
    while(fabs(currentError) > 0.1 && elapsed_time < timeOut) {

        if(fabs(currentError - ENDPOINT) < 1) {
//            initialSlew = -(1 - (ENDPOINT - currentError)/**(distance*0.5)*/);
            if (fabs(distance) < 3) {
                initialSlew = std::min(fabs(ENDPOINT - currentError) + 0.6, 1.0); //multiply scalar from 0.6 >> 1 for small movements
            } else if (fabs(distance) > 5) {
                initialSlew = std::min(fabs(ENDPOINT - currentError) + 0.2, 1.0); //multiply scalar from 0.2 >> 1 for large movements
            } else {
                initialSlew = std::min(fabs(ENDPOINT - currentError) + 0.3, 1.0); //multiply scalar from 0.3 >> 1 for medium movements
            }
        } else {
//            initialSlew = 0;
            initialSlew = 1;
        }

        if(fabs(currentError) < INTEGRAL_LIMIT) {
            totalError += currentError;
        } else {
            totalError = 0;
        }
        //actual pid value and multiply by .5 while in first foot of movement
        //percent of distance of first foot
        if(previousError * currentError <= 0) { //integral is not meant to slow the movement, it should wither do nothing or speed it up
            totalError = 0;
        }

        double p = kP * currentError; //proportional speed control
        double i = kI * totalError; //integral speed control
        double d = kD * (currentError - previousError); //derivative speed control
        double s = kS * steeringError; //steering correction
        double is = kIS * initialSlew; //proportional initial speed control

        std::cout << "p: " << p;
        std::cout << " i: " << i;
        std::cout << " d: " << d;
        std::cout << " s: " << s;
        std::cout << " is: " << is <<std::endl;

        if (fabs(p+i+d) < 15) {
            if (p+i+d > 0) {
                baseMove(15, 15);
            } else {
                baseMove(-15, -15);
            }
        } else {
            baseMove(((p+i+d)*is)-s, ((p+i+d)*is)+s);
        }

        currentValue = getDriveMotorsEncoderAvg();
        previousError = currentError;
        currentError = ENDPOINT - currentValue;
        steeringError = initialHeading - getRotation();
        elapsed_time = pros::millis() - start_time;
        pros::delay(20);
    }
    baseMove(0,0);
}

void autoFunctionTester (int current) {
    std::cout << "auto tester" << std::endl;
    basePIDTurn(current, 2000);
}



// Direction Correction
// Use the inertial sensor and a desired heading to correct steering errors
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
//
//
