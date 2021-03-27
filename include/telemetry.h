#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include "api.h"

#ifdef __cplusplus
extern "C" {

#define TASK_DELAY_SHORT 6

// define compass points as if the robot is facing "north" at 0 degrees.
// increasing degrees are to the right. The four compas points are expressed
// in tenths of degrees
#define N 0
#define E 900
#define S 1800
#define W 2700

extern pros::ADIPort selector;

//tasks
void telemetryGetTaskInit();
void motorTester();

//auton getters
double getBaseEncodersAvg();
double getBaseEncoderRotation();
double getDriveMotorsEncoderAvg();
double getDriveMotorsEncoderRotation();
double getRotation();
double motorEncoderGet(pros::Motor fuckImBroken);
void colorModeSet(bool color);


//chute control getters
bool ballPos1Get();
bool ballPos2Get();
bool ballPos3Get();
bool ballLeaveGet();
bool ballPos1ColorGet();
int ballPos2ColorGet();
bool ballPos3ColorGet();
bool colorModeGet();
void beginTimer(bool yes);
int timeGet();

//debug getters
double encoderGet(pros::ADIEncoder encoder);


#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * You can add C++-only headers here
 */

#endif

#endif  // _TELEMETRY_H_
