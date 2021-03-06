#include "main.h"
#include "telemetry.h"
#include "definitions.h"
#include "motors.h"

pros::Mutex encoders_mutex;

//base encoders (tracking wheels)

pros::ADIEncoder left_base_encoder(LEFT_BASE_ENCODER_INPUT_PORT, LEFT_BASE_ENCODER_OUTPUT_PORT, false);
pros::ADIEncoder right_base_encoder(LEFT_BASE_ENCODER_INPUT_PORT, LEFT_BASE_ENCODER_OUTPUT_PORT, false);
pros::ADIPort selector(SELECTOR_PORT);

double encoderValue;
double baseEncoderAvg;
double baseEncoderRotation;
double driveMotorEncoderAvg;
double driveMotorEncoderRotation;

//stores calculated encoder values into diff variables that can be accessed by the following functions
void baseEncodersGet(void* param) {
  std::uint32_t now = pros::millis();
  while (true) {
    if (encoders_mutex.take(MUTEX_WAIT_SHORT)) {
      //takes avg of two base encoders --> used for forward backward movements
      baseEncoderAvg = (left_base_encoder.get_value() + right_base_encoder.get_value())/2;

      //takes difference of two drive encoders --> used for identifying stray robot turns or directed turns
      baseEncoderRotation = right_base_encoder.get_value() - left_base_encoder.get_value();

      //translation to feet from rotation
      //blue inserts: conversion factor found through testing
      //2 trial on 6 ft of movement and 1 ft of movement >> 3528ticks/6ft = 588ticks/1ft
      driveMotorEncoderAvg = (left_front_mtr.get_position() +
                              left_middle_mtr.get_position() +
                              left_back_mtr.get_position() +
                              right_front_mtr.get_position() +
                              right_middle_mtr.get_position() +
                          right_back_mtr.get_position())/584/6;
      // printf("%i, %6.1f\n", pros::millis(), driveMotorEncoderAvg);
      // pros::lcd::print(4, "Movement Feet: %6.1f", driveMotorEncoderAvg);

      //blue inserts: conversion factor found through testing
      //5 trials on 90 degree rotations compared to the inertial data >> Average 950ticks/90degrees = 10.6ticks/90degrees
      driveMotorEncoderRotation = (left_front_mtr.get_position() +
                              left_middle_mtr.get_position() +
                              left_back_mtr.get_position())/10.6/3 -
                              (right_front_mtr.get_position() +
                              right_middle_mtr.get_position() +
                              right_back_mtr.get_position())/10.6/3;
      encoders_mutex.give();
      }
    pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
  }

}

//encoderGet can be called for debugging if a base encoder is acting strange
double encoderGet(pros::ADIEncoder encoder) {
  encoderValue = encoder.get_value();
  return encoderValue;
}

double motorEncoderGet(pros::Motor fuckImBroken) {
    encoderValue = fuckImBroken.get_position();
    return encoderValue;
}

//returns the average of all 6 base encoders as a double
double getBaseEncodersAvg() {
  return baseEncoderAvg;
}

//returns the difference of the average of 3 base encoders on each side of the base
double getBaseEncoderRotation() {
  return baseEncoderRotation;
}

//returns the average of all 6 motor encoders as a double
double getDriveMotorsEncoderAvg() {
  return driveMotorEncoderAvg;
}

//returns the difference of the average of 3 motor encoders on each side of the base
double getDriveMotorsEncoderRotation() {
  return driveMotorEncoderAvg;
}




//inertial tracking (rotation only right now)

pros::Mutex inertial_mutex;

pros::Imu inertial(INERTIAL_PORT);

double inertialRotation;

void inertialGet(void* param) {
  std::uint32_t now = pros::millis();
  while (true) {
    if (inertial_mutex.take(MUTEX_WAIT_SHORT)) {
      //set gyroRotation to the inertial sensor's output
      inertialRotation = -inertial.get_rotation();
      // printf("%i, %6.1f\n", pros::millis(), inertialRotation);
      pros::lcd::print(6, "Rotation Degrees: %6.1f", inertialRotation);
      inertial_mutex.give();
    }
    pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
  }
}

void tareInertial() {
    inertial.tare_rotation();
    pros::delay(300);
}

double getRotation() {
  return inertialRotation;
}


//internal ball tracking

pros::Mutex ball_tracking_mutex;

bool ballPos1 = false; //false is no ball present
bool ballPos2 = false;
bool ballPos3 = false;
bool ballLeave = false;
bool ballShoot = false;
bool ballPos1Color = RED; //RED == true, BLUE == false
int ballPos2Color = 0;
bool ballPos3Color = RED;
bool colorMode = RED;
int a;


//Set all Chute data to booleans
void chuteGet(void* pointerParam) {
  std::uint32_t now = pros::millis();

  while (true) {
    if (ball_tracking_mutex.take(MUTEX_WAIT_SHORT)) {

      //Chute Sensor Declarations
      pros::ADILineSensor lower_line_sensor(LOWER_LINE_SENSOR_PORT);
      pros::ADILineSensor upper_line_sensor(UPPER_LINE_SENSOR_PORT);
      pros::ADILineSensor secondPos_line_sensor(EJECT_LINE_SENSOR_PORT);
      pros::ADIUltrasonic eject_sensor(ULTRA_PING_PORT, ULTRA_ECHO_PORT);
      pros::Optical optical_sensor(OPTICAL_SENSOR_PORT);
      pros::Optical optical_high_sensor(OPTICAL_HIGH_SENSOR_PORT);
      pros::Distance leave_distance(LEAVE_DISTANCE_SENSOR_PORT);
      pros::Distance shoot_distance(SHOOT_DISTANCE_SENSOR_PORT);
      pros::Distance high_distance(HIGH_DISTANCE_SENSOR_PORT);


        optical_sensor.set_led_pwm(100);
        optical_high_sensor.set_led_pwm(100);


        a = master.get_digital_new_press(DIGITAL_A);

      if (a) {
          if (colorMode == EJECT_RED) {
              master.rumble(".");
              colorMode = EJECT_BLUE;
              master.print(0, 0, "EJECTING BLUE");
//              master.set_text(0, 0,"proximity: %d", optical_sensor.get_proximity());
          } else {
              master.rumble("..");
              colorMode = EJECT_RED;
              master.print(0, 0, "EJECTING RED");

          }
      }



      // std::cout << "Lower Line Sensor: " << (lower_line_sensor.get_value())<< std::endl;
      // std::cout << "Upper Line Sensor: " << (upper_line_sensor.get_value())<< "\n" << std::endl;

      // printf("%i, %d\n", pros::millis(), upper_line_sensor.get_value());
      // printf("%i value: %i\n", pros::millis(), static_cast<int>(upper_line_sensor.get_value()));

      int ballPos1Avg[5];
      int ballPos2Avg[5];
      int ballPos3Avg[5];
      int ballLeaveAvg[5];
      int colorAvg[5];
      int ballShootAvg[5];
      int total1 = 0;
      int total2 = 0;
      int total3 = 0;
      int totalL = 0;
      int totalC = 0;
      int totalS = 0;
      int avg1 = 0;
      int avg2 = 0;
      int avg3 = 0;
      int avgL = 0;
      int avgC = 0;
      int avgS = 0;

      int opticalAvg[2];
      int totalOptical;
      int avgOptical;

      for (int i=0; i<5; i++) {
          ballPos1Avg[i] = lower_line_sensor.get_value();
          colorAvg[i] = optical_sensor.get_hue();
          ballPos3Avg[i] = upper_line_sensor.get_value();
          ballPos2Avg[i] = secondPos_line_sensor.get_value();
//          ballLeaveAvg[i] = secondPos_line_sensor.get_value();
          ballLeaveAvg[i] = leave_distance.get();
          ballShootAvg[i] = shoot_distance.get();
          pros::delay(20);
      }



            for (int i=0; i<5; i++) {
          total1 += ballPos1Avg[i];
          total2 += ballPos2Avg[i];
          total3 += ballPos3Avg[i];
          totalL += ballLeaveAvg[i];
          totalC += colorAvg[i];
          totalS += ballShootAvg[i];

      }

      avg1 = total1/5;
      avg2 = total2/5;
      avg3 = total3/5;
      avgL = totalL/5;
      avgC = totalC/5;
      avgS = totalS/5;

        avgOptical = totalOptical/5;

        for (int i=0; i<2; i++) {
            opticalAvg[i] = optical_sensor.get_hue();
            pros::delay(20);
        }

        for (int i=0; i<2; i++) {
            totalOptical += opticalAvg[i];

        }

        avgOptical = totalOptical/2;

      //setter logic

      if (optical_sensor.get_proximity() > 200) {
          ballPos2 = true;
          if (optical_sensor.get_hue() < 25) { //if red ball
              ballPos2Color = 1;
          } else if (optical_sensor.get_hue() > 180) { //if blue ball
              ballPos2Color = 2;
          }
      } else {
          ballPos2Color = 0;
          ballPos2 = false;
      }

       if (optical_high_sensor.get_proximity() > 50) {
           ballPos3 = true;
           if (optical_high_sensor.get_hue() < 100) { //if red ball
               ballPos3Color = 1;
           } else if (optical_high_sensor.get_hue() > 100) { //if blue ball
               ballPos3Color = 2;
           }
       } else {
            ballPos3Color = 0;
            ballPos3 = false;
        }


//      if(avg2 < 2900) {
//          ballPos2 = true;
//      } else {
//          ballPos2 = false;
//      }

      if(avg1 < 2900){ //if ball is present
        ballPos1 = true;
      } else {
        ballPos1 = false;
      }

//      if(avg3 < 2900){ //if ball is present
//        ballPos3 = true;
//      } else {
//        ballPos3 = false;
//      }

        if(high_distance.get() < 60 && high_distance.get() > 0) { //if ball is present
            ballPos3 = true;
//          std::cout <<"set L true" << std::endl;
        } else {
            ballPos3 = false;
//          std::cout <<"set L false" << std::endl;
        }

      if(leave_distance.get() < 60 && leave_distance.get() > 0) { //if ball is present
          ballLeave = true;
//          std::cout <<"set L true" << std::endl;
      } else {
          ballLeave = false;
//          std::cout <<"set L false" << std::endl;
      }

      if(shoot_distance.get() < 60 && shoot_distance.get() > 0) { //if ball is present (values in mm)
          ballShoot = true;
//          std::cout << "set S true" << std::endl;
      } else {
          ballShoot = false;
//          std::cout << "set S false" << std::endl;
      }




      pros::lcd::print(0, "Position 3: %d", ballPos3Get());
      pros::lcd::print(1, "Leave: %d", ballLeaveGet());
//      pros::lcd::print(2, "Position 3: %d", avg3);
      pros::lcd::print(3, "proximity: %d", optical_sensor.get_proximity());
//      pros::lcd::print(3, "color avg: %d", avgO\/\\ptical;
      pros::lcd::print(4, "Color: %f", optical_sensor.get_hue());
//      pros::lcd::print(4, "Eject: %d", ballLeaveGet());
      pros::lcd::print(2, "Shoot: %d", ballShootGet());

//      std::cout << "hue " << optical_high_sensor.get_hue() << std::endl;
//        std::cout << "proximity "<< optical_high_sensor.get_proximity() << std::endl;
//        std::cout << optical_sensor.get_hue() << std::endl;


//      pros::lcd::print(5, "Eject: %d", eject_sensor.get_value());
//      pros::lcd::print(6, "Selector %d", selector.get_value());

        if (colorMode == EJECT_RED) {
            pros::lcd::print(5, "EJECTING RED");
        } else {
            pros::lcd::print(5, "EJECTING BLUE");


//            master.set_text(pros::E_CONTROLLER_MASTER, 0, 0, "EJECTING BLUE");
        }



//    if (chuteDirectionGet() && ballPos2) { //if intakeIn
//      ballPos2Color = ballPos1Color;
//    }


    //two states
    //if a ball is present, what color is it


    //INTAKE AND INDEX LOGIC >> Assume color system integrated with the motor movements
    //begin >> no balls in bot, all three sensors read neg on present
    //ball enters bot
      //identify its color >> index it too the top, or spit it out

    //one ball at top, we can assume it is the correct color because it would have been ejected otherwise
    //ball enters bot
      //identify its color >> index it to middle, or eject it (should the eject logic not come into play until its reached the middle state?)

    //two balls in robot, they both will be the correct color if this state has been reached
    //ball enters bot
      //identify its color?
      //do nothing with it and just index it to the next ready position?





    //INTAKE ANS INDEX LOGIC >> Know color system (just telemetry read outs)
    //begin >> no balls in bot, all three sensors read neg on present
    //ball enters bot
      //identify its color as it enters and assign it a BLUE or RED value
      //if the intakes are running in and a ball arrives at a new ballpos, we can assume it came from the below ballpos
      //if the intakes are running out and a ball arrives at a new ballpos, we can assume it came from the above ballpos
        //based on those two logic bits, assign a true false and red blues identifier to each pos
        //use this data to govern the ball movements


      ball_tracking_mutex.give();
    }
    pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
  }
}



double time;
bool bigBang;

void beginTimer(bool yes) {
    bigBang = yes;
}

void timer(void* param) {
    std::uint32_t now = pros::millis();
    int start_time = pros::millis();
    time = 0;
    while (true) {
        if (bigBang) {
            time = 0;
            start_time = pros::millis();
            bigBang = false;
            std::cout << "Time begin:" << time << std::endl;
        }
//        std::cout << "Time now:" << time << std::endl;
        time = (pros::millis() - start_time);
        pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }
}

int timeGet() {
    return time;
}

bool ballPos1Get() {
  return ballPos1;
}

bool ballPos2Get() {
  return ballPos2;
}

bool ballPos3Get() {
  return ballPos3;
}

bool ballLeaveGet() {
 return ballLeave;
}

bool ballPos1ColorGet() {
  return ballPos1Color;
}

int ballPos2ColorGet() {
  return ballPos2Color;
}

bool ballPos3ColorGet() {
  return ballPos3Color;
}

bool ballShootGet() {
    return ballShoot;
}

void colorModeSet(bool color) {
    if (EJECT_RED) {
//        master.clear_line(0);
        colorMode = EJECT_RED;
    } else {
//        master.clear_line(0);
        colorMode = EJECT_BLUE;
    }
}

bool colorModeGet() {
    return colorMode;
}




// double absGyroValue;
// double gyroValue;
// okapi::ADIGyro gyro(BASE_GYRO_PORT);
//
// void gyroGet(void* param) {
// 	/*
// 	Thanks to jpearman for code to generate absGyroValue from
//   https://www.vexforum.com/index.php/36049-gyro-acting-weird-v5
// 	*/
// 	double value = 0;
// 	double lastValue = 0;
// 	double delta = 0;
// 	int invert = -1;
//
//   std::uint32_t now = pros::millis();
// 	while (true) {
// 		value = gyro.controllerGet();
//     // printf("%i raw gyro value: %6.1f\n", pros::millis(), value);
//     // printf("%i value: %i\n", pros::millis(), static_cast<int>(value));
// 		if (value > -4000 && value < 4000) {
//       value = value * invert;
// 			// calculate change in gyro value
// 	    int delta = value - lastValue;
// 	    lastValue = value;
//
// 	    // delta should be small, did we overflow gyro value?
// 	    if( delta > 1800 )
// 	      delta -= 3600;
// 	    if( delta < -1800 )
// 	      delta += 3600;
//
// 	    // add change to absolute value
// 			gyroValue = value;
// 	    absGyroValue += delta;
//     }
//
// 		pros::Task::delay_until(&now, TASK_DELAY_SHORT);
// 	}
// }



void telemetryGetTaskInit() {
	// pros::Task gyro_task(gyroGet,(void*)"GYRO_TASK");

  // pros::Task encoder_task(baseEncodersGet,(void*)"ENCODER_TASK");
  // pros::Task inertial_task(inertialGet,(void*)"INERTIAL_TASK");
  // pros::Task chute_tele_task(chuteGet,(void*)"CHUTE_TELE_TASK");

  pros::Task encoders_task(baseEncodersGet,(void*)"BASE_TASK");
  pros::Task ball_tracking_task(chuteGet,(void*)"BALL_TRACKING_TASK");
  pros::Task inertial_task(inertialGet,(void*)"INERTIAL_TASK");
  pros::Task timer_task(timer,(void*)"TIMER_TASK");
}

void timerTaskInit() {
    pros::Task timer_task(timer,(void*)"TIMER_TASK");
}

// double gyroGetAbsolute() {
//   return absGyroValue;
// };
//
// double gyroGetLocal() {
//   return gyroValue;
// };

//ENCODERS
