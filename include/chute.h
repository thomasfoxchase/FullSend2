#ifndef _CHUTE_H_
#define _CHUTE_H_

#include "main.h"

#ifdef __cplusplus
extern "C" {

extern pros::Motor chute_mtr;

void chuteControlTaskInit();

void chuteMove(int chutePower);

#endif
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * You can add C++-only headers here
 */

#endif

#endif  // _CHUTE_H_
