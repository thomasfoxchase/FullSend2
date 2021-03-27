#ifndef _CHUTE_H_
#define _CHUTE_H_

#include "main.h"

#ifdef __cplusplus
extern "C" {

extern pros::Motor chute_mtr;

void chuteControlTaskInit();
void intakeMove(int sideRollerPower);
void chuteEject(int chutePower, int indexPower);
void chuteIndex(int chutePower);
void chuteMove(int chutePower);
void lessSmartEjectCycle();
void autoShootUntil(bool color, int time_limit);
void setEjectIt(bool yes);
void chuteAutoTaskInit();
void autoShootOneBall();
void setShootCycle(bool shootme);
void outdexFix();




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
