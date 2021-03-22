#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include "main.h"

#ifdef __cplusplus
extern "C" {

//CONTROLLER DEFINITIONS
extern pros::Controller master;
extern pros::Controller buddy;


//SENSOR PORTS
#define LEFT_BASE_ENCODER_INPUT_PORT 'f'
#define LEFT_BASE_ENCODER_OUTPUT_PORT 'f'
#define RIGHT_BASE_ENCODER_INPUT_PORT 'g'
#define RIGHT_BASE_ENCODER_OUTPUT_PORT 'd'
#define UPPER_LINE_SENSOR_PORT 'b'
#define LOWER_LINE_SENSOR_PORT 'c'
#define EJECT_LINE_SENSOR_PORT 'd'
#define INERTIAL_PORT 21
#define OPTICAL_SENSOR_PORT 8
#define SELECTOR_PORT 'a'
#define ULTRA_PING_PORT 'e'
#define ULTRA_ECHO_PORT 'f'


//current values
#define DEFAULT_CURRENT 2500
#define AUTON_CURRENT 1500

//power values
#define FULL_POWER 127
#define HALF_POWER 64

//motor properties
#define REVERSE true
#define IN true
#define OUT false
#define RED true
#define BLUE false
#define KEEPME true
#define EJECTME false

//delays
#define MUTEX_WAIT_SHORT 2
#define TASK_DELAY_NORMAL 20


#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * You can add C++-only headers here
 */

#endif

#endif  // _DEFINITIONS_H_
