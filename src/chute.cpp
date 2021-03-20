#include "chute.h"
#include "telemetry.h"
#include "motors.h"
#include "definitions.h"

pros::Mutex chute_mutex;
bool chuteDirection = true;



void chuteMove(int chutePower) {
  ejector_mtr = chutePower;
  ejector_mtr2 = chutePower;
  lower_chute_mtr = chutePower;
}

void chuteIndex(int chutePower) {
  lower_chute_mtr = chutePower;
}

void chuteEject(int chutePower) {
  ejector_mtr = -chutePower;
  ejector_mtr2 = -chutePower;
  lower_chute_mtr = chutePower;
}

void intakeMove(int sideRollerPower) {
  right_intake_mtr = sideRollerPower;
  left_intake_mtr = sideRollerPower;
}

int right1;
int right2;
int left1;
int left2;

void chuteControl(void* param) {
  //manage chute
  std::uint32_t now = pros::millis();
  double maxTimeBeforeSecondClick = 200;
  double chuteClickTime = 0;
  while (true) {
    right1 = master.get_digital(DIGITAL_R1);
    right2 = master.get_digital(DIGITAL_R2);
    left1 = master.get_digital(DIGITAL_L1);
    left2 = master.get_digital(DIGITAL_L2);
    if (chute_mutex.take(MUTEX_WAIT_SHORT)) {
      if (left2) { //rollers in
        chuteEject(127); //eject balls out back
      } else if (left1) {
        intakeMove(FULL_POWER);
        chuteIndex(127); //shoot balls
        chuteDirection = IN;
      } else if (right1) {
        chuteMove(127); //intake balls
        chuteDirection = IN;
      } else if (right2) {
        intakeMove(-FULL_POWER);
        chuteMove(-127); //eject out bottom
        chuteDirection = OUT;
      } else {
      chuteMove(0);
      intakeMove(0);
      }
      chute_mutex.give();
     }
   pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
 }
}


void chuteSmartControl(void* param) {
  std::uint32_t now = pros::millis();

  //INTAKE AND INDEX LOGIC >> Know color system (just telemetry read outs)
  //begin >> no balls in bot, all three sensors read neg on present
  //ball enters bot
    //identify its color as it enters and assign it a BLUE or RED value
    while(true) {
      if (chute_mutex.take(MUTEX_WAIT_SHORT)) {
        if (ballPos2Get()) {
          if (ballPos2ColorGet() == RED) { //this will be adjusted later to be customizable based on match
            chuteEject(127);
          }
        } else if (ballPos1Get()) {
          if (!ballPos3Get()) { //there are no balls in the chute
            while(!ballPos3Get()) {
              chuteIndex(127); //index to ballPos3
            }
            chuteIndex(0);
          } else if (ballPos3Get()) { //there is one ball in the chute
            while(!ballPos2Get()) {
              chuteIndex(127); //index to ballPos2
            }
            chuteIndex(0);
          } else {
            chuteIndex(127); //index ball into bot slightly
            pros::delay(200);
            chuteIndex(0);
          }

        }



        chute_mutex.give();
      }
      pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }

    //if the intakes are running in and a ball arrives at a new ballpos, we can assume it came from the below ballpos
    //if the intakes are running out and a ball arrives at a new ballpos, we can assume it came from the above ballpos
      //based on those two logic bits, assign a true false and red blues identifier to each pos
      //use this data to govern the ball movements




}



void chuteControlTaskInit() {
pros::Task chute_task(chuteControl,(void*)"DUMMY");
}
