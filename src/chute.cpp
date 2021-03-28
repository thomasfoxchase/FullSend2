#include "chute.h"
#include "telemetry.h"
#include "motors.h"
#include "definitions.h"

pros::Mutex chute_mutex;
pros::Mutex intake_mutex;
pros::Mutex ejector_mtrs_mutex;

bool chuteDirection = true;
bool ejectIt = false;
bool shootCycle = false;


void chuteMove(int chutePower) {
    if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
        ejector_mtr = chutePower;
        ejector_mtr2 = chutePower;
        lower_chute_mtr = (double) chutePower * .7;
        flywheel_mtr = chutePower;
        ejector_mtrs_mutex.give();
    }
}

void chuteMoveOut(int chutePower) {
    lower_chute_mtr = (double) -chutePower*.7;
    flywheel_mtr = -chutePower;
}

void chuteShoot(int chutePower) {
    if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
        ejector_mtr = chutePower;
        ejector_mtr2 = chutePower;
        flywheel_mtr = chutePower;
        ejector_mtrs_mutex.give();
    }
}

void chuteIndex(int chutePower) {
  lower_chute_mtr = (double) chutePower*.7;
}

void chuteEject(int chutePower, int indexPower) {
    if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
        ejector_mtr = -chutePower;
        ejector_mtr2 = -chutePower;
        lower_chute_mtr = (double) indexPower * .7;
        ejector_mtrs_mutex.give();
    }
}

void intakeMove(int sideRollerPower) {
  right_intake_mtr = sideRollerPower;
  left_intake_mtr = sideRollerPower;
}

//for auto
void setEjectIt(bool yes) {
    ejectIt = yes;
}

void setShootCycle(bool shootme) {
    shootCycle = shootme;
}


//void autoEjectNoBlock(void* param) {
//    std::uint32_t now = pros::millis();
//    while (true) {
//        if (ejectIt) {
//            if (ballPos2ColorGet() == 1 && colorModeGet() == EJECT_RED) { //eject red balls
//                std::cout << "eject red" << std::endl;
//                lessSmartEjectCycle();
//            } else if (ballPos2ColorGet() == 2 && colorModeGet() == EJECT_BLUE) { //eject blue balls
//                std::cout << "eject blue" << std::endl;
//                lessSmartEjectCycle();
//            }
//        }
//    pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
//    }
//}



//void autoShootUntil(bool color, int time_limit) {
//    int start_time = pros::millis();
//    int elapsed_time = 0;
//    if (color) {
//        while (ballPos2ColorGet() != 2 && elapsed_time < time_limit) {
//            chuteMove(127);
//            pros::delay(20);
//            elapsed_time = pros::millis() - start_time;
//        }
//    } else {
//        while (ballPos2ColorGet() != 1 && elapsed_time < time_limit) {
//            chuteMove(127);
//            pros::delay(20);
//            elapsed_time = pros::millis() - start_time;
//        }
//    }
//}

void outdexFix() {
    int elapsed_time = 0;
    int start_time = pros::millis();
    while(!ballShootGet() && elapsed_time < 1000) {
        chuteMoveOut(100);
        intakeMove(-20);
        elapsed_time = pros::millis() - start_time;
        pros::delay(20);
    }
    chuteMoveOut(0);
    intakeMove(0);
    elapsed_time = 0;
    start_time = pros::millis();
    while(!ballPos3Get() && elapsed_time < 1000) {
        chuteIndex(90);
        intakeMove(50);
        elapsed_time = pros::millis() - start_time;
        pros::delay(20);
    }
    chuteIndex(0);
}

void autoShootOneBall() { //run outdexFix() first
    int elapsed_time = 0;
    int start_time = pros::millis();
    while (!ballShootGet() && elapsed_time < 1000) {
        chuteShoot(127);
        elapsed_time = pros::millis() - start_time;
        pros::delay(20);
    }
    pros::delay(500);
    chuteShoot(-20);
    pros::delay(100);
    chuteShoot(0);

//    chuteMoveOut(50);
//    pros::delay(500);
//    chuteMoveOut(0);
//    chuteIndex(70);
//    pros::delay(700);
//    chuteIndex(0);
//    pros::delay(500);
//    flywheel_mtr = 127;
//    pros::delay(500);
//    ejector_mtr = 50;
//    ejector_mtr2 = 50;
//    pros::delay(500);
//    flywheel_mtr = -20; //brake
//    ejector_mtr = -20;
//    ejector_mtr2 = -20;
//    lower_chute_mtr = -20;
//
//    pros::delay(200);
//    flywheel_mtr = 0;
//    ejector_mtr = 0;
//    ejector_mtr2 = 0;
//    lower_chute_mtr = 0;
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
        std::cout << "im in chuteControl" << std::endl;
        if (left2) { //rollers in
        chuteEject(127, 127); //eject balls out back
      } else if (left1) {
        intakeMove(FULL_POWER);
        chuteIndex(127); //intake balls
        chuteDirection = IN;
      } else if (right1) {
        chuteMove(127); //shoot balls
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

void intakeControl(void* param) {
    //manage side rollers
    std::uint32_t now = pros::millis();
    while(true) {
        left1 = master.get_digital(DIGITAL_L1);
        left2 = master.get_digital(DIGITAL_L2);
        if (intake_mutex.take(MUTEX_WAIT_SHORT)) {
            if (left1) {
                intakeMove(127);
            } else if (left2) {
                intakeMove(-127);
            } else {
                intakeMove(0);
            }
            intake_mutex.give();
        }
        pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }
}


void chuteSmartIndexingControl(void* param) {
  std::uint32_t now = pros::millis();

  //INTAKE AND INDEX LOGIC >> Know color system (just telemetry read outs)
  //begin >> no balls in bot, all three sensors read neg on present
  //ball enters bot
    //identify its color as it enters and assign it a BLUE or RED value

    //CONTROLS:
    //left1 = shoot balls
    //left2 = down take chute
    //right1 = side rollers in
    //right2 = side rollers out
    //left2 && right2 = eject out bottom
    //left1 && right2 = shoot balls while putting last ball in (if 4 balls are in the chute >> the last will be in the intakes)

    while(true) {
        left1 = master.get_digital(DIGITAL_L1);
        left2 = master.get_digital(DIGITAL_L2);
        if (chute_mutex.take(MUTEX_WAIT_SHORT)) {
          if (left1) { //shoot balls
            std::cout << "shoot balls" << std::endl;
            if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
                chuteMove(127);
                ejector_mtrs_mutex.give();
            }
        } else if (left2) { //for ejecting out bottom if pressed with right 2 or failsafe for misfire balls
            if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
                chuteMove(-127);
                ejector_mtrs_mutex.give();
            }
        } else if (ballPos1Get()) {
          if (!ballPos3Get()) { //there are no balls in the chute
            std::cout << "indexing first ball" << std::endl;
            while(!ballPos3Get() && ballPos2ColorGet() != 1) {
              chuteIndex(127); //index to ballPos3
              pros::delay(20);
            }
            chuteIndex(0);
          } else if (ballPos3Get()) { //there is one ball in the chute
              std::cout << "indexing second ball" << std::endl;
              while(!ballPos2Get() && ballPos2ColorGet() != 1) {
              chuteIndex(127); //index to ballPos2
              pros::delay(20);
            }
            chuteIndex(0);
          } else { //there are two balls already in the robot
            std::cout << "indexing third ball" << std::endl;
            chuteIndex(127); //index ball into bot slightly
            pros::delay(200);
            chuteIndex(0);

          }
        } else {
//            std::cout << "do nothing" << std::endl;
            if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
                chuteMove(0);
                chuteEject(0,0);
                ejector_mtrs_mutex.give();
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

void ejectControl(void* param) {
    std::uint32_t now = pros::millis();
    int start_time = pros::millis();
    int elapsed_time;
    double scalar = 1;
    while(true) {
        if (ballPos2ColorGet() == 1) { //if red ball: this will be adjusted later to be customizable based on match
            if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
                std::cout << "eject ball" << std::endl;
                elapsed_time = 0;
                while (!ballLeaveGet() && elapsed_time < 2000 ) {
                    std::cout << elapsed_time << std::endl;
                    if (ballPos1Get()) {
                        scalar = 0.5;
                    } else {
                        scalar = 1;
                    }
                    chuteEject(127,127*scalar); //eject until the ball is seen leaving
                    pros::delay(20);
                    elapsed_time = pros::millis() - start_time;
                }
                chuteEject(-127,0);
                pros::delay(100);
//                motorBrake(ejector_mtr, ejector_mtr2);
                chuteEject(0,0);
//            } else {
//                std::cout << "the ball was blue" << std::endl;
                ejector_mtrs_mutex.give();
            }
        }
        pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }
}

void lessSmartEjectCycle(){
    std::cout << "eject ball cycle" << std::endl;
    int start_time = pros::millis();
    int elapsed_time = 0;
    double scalar = 1;
//    if(ballPos3Get()) {
//        chuteEject(-70,127);
//        pros::delay(200);
//    }
    if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
        std::cout << ballLeaveGet() << std::endl;
        while (!ballLeaveGet() && elapsed_time < 1000) {
            std::cout << elapsed_time << std::endl;
            if (ballPos1Get()) {
                scalar = 0.5;
            } else {
                scalar = 1;
            }
            chuteEject(127, 127 * scalar); //eject until the ball is seen leaving
            elapsed_time = pros::millis() - start_time;
            pros::delay(20);
        }
        chuteEject(-50, 0);
        pros::delay(20);
        chuteEject(0, 0);
        ejector_mtrs_mutex.give();
    }
}



void chuteLessSmartControl(void* param) {
    std::uint32_t now = pros::millis();
    while(true) {
        left1 = master.get_digital(DIGITAL_L1);
        left2 = master.get_digital(DIGITAL_L2);
        right1 = master.get_digital(DIGITAL_R1);
        right2 = master.get_digital(DIGITAL_R2);
        if (chute_mutex.take(MUTEX_WAIT_SHORT)) {
            if (ballPos2ColorGet() == 1 && colorModeGet() == EJECT_RED) { //eject red balls
                std::cout << "eject red" << std::endl;
                lessSmartEjectCycle();
            } else if (ballPos2ColorGet() == 2 && colorModeGet() == EJECT_BLUE) { //eject blue balls
                std::cout << "eject blue" << std::endl;
                lessSmartEjectCycle();
            } else if (left1 && !right1) { //index balls
                std::cout << "index balls" << std::endl;
                chuteIndex(127);
            } else if (right1) { //shoot balls
                std::cout << "shoot balls" << std::endl;
//                if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
                chuteMove(127);
//                    ejector_mtrs_mutex.give();
//                }
            } else if (right2) {
                chuteMove(-127);
//            } else if (left2) { //for ejecting out bottom if pressed with right 2 or failsafe for misfire balls
////                if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
//                    std::cout << "spit balls" << std::endl;
//                    chuteMove(-127);
////                    ejector_mtrs_mutex.give();
////                }
            } else {
//            std::cout << "do nothing" << std::endl;
//                if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
                    chuteMove(0);
                    chuteEject(0,0);
//                    ejector_mtrs_mutex.give();
//                }

            }
            chute_mutex.give();
        }
        pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }
}


void chuteControlTaskInit() {
//    pros::Task chute_task(chuteSmartIndexingControl,(void*)"CHUTE_TASK");
//    pros::Task eject_task(ejectControl,(void*)"EJECT_TASK");
    pros::Task intake_task(intakeControl,(void*)"INTAKE_TASK");
//    pros::Task chute_task(chuteControl,(void*)"CHUTE_TASK");
    pros::Task chute_task(chuteLessSmartControl,(void*)"CHUTE_TASK");
}
void chuteAutoTaskInit() {
//    pros::Task chute_auto_task(autoEjectNoBlock,(void*)"CHUTE_AUTO_TASK");
}

