#ifndef _BASE_H_
#define _BASE_H_

#include "main.h"

#ifdef __cplusplus
extern "C" {

extern pros::Mutex base_mutex;

// extern pros::Controller master;

#define DEFAULT_CURRENT 2500


extern pros::Motor left_back_mtr;
extern pros::Motor right_back_mtr;
extern pros::Motor left_front_mtr;
extern pros::Motor right_front_mtr;
// extern pros::Controller master(pros::E_CONTROLLER_MASTER);

void baseControlTaskInit();
void baseTester();
void autoFunctionTester(int current);
void basePIDTurn(double rotation, int timeOut);
void basePIDMove(double distance, int timeOut);


#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * You can add C++-only headers here
 */

#endif

#endif  // _BASE_H_
