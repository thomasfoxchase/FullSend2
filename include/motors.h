#ifndef _MOTORS_H_
#define _MOTORS_H_

#include "main.h"

#ifdef __cplusplus
extern "C" {

bool chuteDirectionGet();
void motorBrake(pros::Motor motor, pros::Motor motor2);

//motor definitions
extern pros::Motor left_front_mtr;
extern pros::Motor right_front_mtr;
extern pros::Motor right_back_mtr;
extern pros::Motor left_back_mtr;
extern pros::Motor right_middle_mtr;
extern pros::Motor left_middle_mtr;
extern pros::Motor lower_chute_mtr;
extern pros::Motor upper_chute_mtr;
extern pros::Motor left_intake_mtr;
extern pros::Motor right_intake_mtr;
extern pros::Motor ejector_mtr;
//extern pros::Motor ejector_mtr2;
extern pros::Motor flywheel_mtr;
extern pros::Motor flywheel_mtr2;


//MOTOR PORTS

  //Base
#define FRONT_RIGHT_DRIVE_PORT 17
#define MIDDLE_RIGHT_DRIVE_PORT 18
#define BACK_RIGHT_DRIVE_PORT 20
#define FRONT_LEFT_DRIVE_PORT 16
#define MIDDLE_LEFT_DRIVE_PORT 14
#define BACK_LEFT_DRIVE_PORT 11

  //Chute
#define LOWER_CHUTE_PORT 7
#define UPPER_CHUTE_PORT 1

  //Intakes
#define LEFT_INTAKE_PORT 1
#define RIGHT_INTAKE_PORT 9

  //Ejector
#define EJECTOR_PORT 4
//#define EJECTOR2_PORT 12

//flywheel
#define FLYWHEEL_PORT 3
#define FLYWHEEL2_PORT 15




#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * You can add C++-only headers here
 */

#endif

#endif  // _MOTORS_H_
