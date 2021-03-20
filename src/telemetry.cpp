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
      // printf("%i, %6.1f\n", pros::millis(), baseEncoderAvg);

      //takes difference of two drive encoders --> used for identifying stray robot turns or directed turns
      baseEncoderRotation = right_base_encoder.get_value() - left_base_encoder.get_value();
      // printf("%i, %6.1f\n", pros::millis(), baseEncoderRotation);


      //more operations to be added here

      //translation to feet from rotation

      //blue inserts: conversion factor found through testing
      //2 trial on 6 ft of movement and 1 ft of movement >> 3528ticks/6ft = 588ticks/1ft
      driveMotorEncoderAvg = (left_front_mtr.get_position() +
                              left_middle_mtr.get_position() +
                              left_back_mtr.get_position() +
                              right_front_mtr.get_position() +
                              right_middle_mtr.get_position() +
                              right_back_mtr.get_position())/588/6;
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
      // printf("%i, %6.1f\n", pros::millis(), driveMotorEncoderRotation);
      // pros::lcd::print(5, "Rotation Ticks: %6.1f", driveMotorEncoderRotation);


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

double getRotation() {
  return inertialRotation;
}


//internal ball tracking

pros::Mutex ball_tracking_mutex;

bool ballPos1 = false; //false is no ball present
bool ballPos2 = false;
bool ballPos3 = false;
bool ballLeave = false;
bool ballPos1Color = RED; //RED == true, BLUE == false
bool ballPos2Color = RED;
bool ballPos3Color = RED;



//Set all Chute data to booleans
void chuteGet(void* pointerParam) {
  std::uint32_t now = pros::millis();

  while (true) {
    if (ball_tracking_mutex.take(MUTEX_WAIT_SHORT)) {

      //Chute Sensor Declarations
      pros::ADILineSensor lower_line_sensor(LOWER_LINE_SENSOR_PORT);
      pros::ADILineSensor upper_line_sensor(UPPER_LINE_SENSOR_PORT);
      pros::ADILineSensor eject_line_sensor(EJECT_LINE_SENSOR_PORT);
      pros::Optical optical_sensor(OPTICAL_SENSOR_PORT);

      // pros::Vision vision_sensor(OPTICAL_SENSOR_PORT);
      // okapi::OpticalSensor optical(OPTICAL_SENSOR_PORT);

      // optical_sensor.disable_gesture();
      optical_sensor.set_led_pwm(50);
      //Print Statements
      pros::lcd::print(1, "Lower Line Sensor: %d", lower_line_sensor.get_value());
      pros::lcd::print(2, "Upper Line Sensor: %d", upper_line_sensor.get_value());
      pros::lcd::print(3, "Selector %d", selector.get_value());
      pros::lcd::print(4, "Optical Value: %f", optical_sensor.get_hue()); //reading out -4194304
      pros::lcd::print(5, "Optical Light: %d", optical_sensor.get_led_pwm()); //reading out 2147483647



      // std::cout << "Lower Line Sensor: " << (lower_line_sensor.get_value())<< std::endl;
      // std::cout << "Upper Line Sensor: " << (upper_line_sensor.get_value())<< "\n" << std::endl;

      // printf("%i, %d\n", pros::millis(), upper_line_sensor.get_value());

      // printf("%i value: %i\n", pros::millis(), static_cast<int>(upper_line_sensor.get_value()));


      //setter logic
      if(optical_sensor.get_hue() > 300) { //if no ball
        ballPos2 = false;
      } else if (optical_sensor.get_hue() > 50 && optical_sensor.get_hue() < 100) { //if red ball
        ballPos2 = true;
        ballPos1Color = RED;
      } else if (optical_sensor.get_hue() > 100 && optical_sensor.get_hue() < 200) { //if blue ball
        ballPos2 = true;
        ballPos1Color = BLUE;
      }



      if(lower_line_sensor.get_value() < 2000){ //if ball is present
        ballPos1 = true;
      } else {
        ballPos1 = false;
      }

      if(upper_line_sensor.get_value() < 2000){ //if ball is present
        ballPos3 = true;
      } else {
        ballPos3 = false;
      }

      if(eject_line_sensor.get_value() < 2000){ //if ball is present
          ballLeave = true;
      } else {
          ballLeave = false;
      }

      if (chuteDirectionGet() && ballPos2) { //if intakeIn
        ballPos2Color = ballPos1Color;
      }


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

bool ballPos2ColorGet() {
  return ballPos2Color;
}

bool ballPos3ColorGet() {
  return ballPos3Color;
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

}

// double gyroGetAbsolute() {
//   return absGyroValue;
// };
//
// double gyroGetLocal() {
//   return gyroValue;
// };

//ENCODERS
