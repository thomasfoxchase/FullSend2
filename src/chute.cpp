#include "chute.h"
#include "telemetry.h"
#include "motors.h"
#include "definitions.h"

pros::Mutex chute_mutex;
pros::Mutex intake_mutex;
pros::Mutex ejector_mtrs_mutex;
pros::Mutex ejector_sequence_mutex;


pros::Mutex ejector_mutex;
pros::Mutex indexer_mutex;
pros::Mutex flyWheel_mutex;



bool chuteDirection = true;
bool ejectIt = false;
bool shootCycle = false;
bool eject_a_ball = false;


void ejectorMove(int ejectorPower) {
    ejector_mtr = ejectorPower;
}

void indexerMove(int indexerPower, double scalar) {
    lower_chute_mtr = (double) indexerPower * scalar; //scaling available

}

void flyWheelMove(int flyWheelPower) {
    flywheel_mtr = flyWheelPower;
    flywheel_mtr2 = flyWheelPower;
}

void ejectorMoveNoMutex(int ejectorPower) {
    ejector_mtr = ejectorPower;
}

void indexerMoveNoMutex(int indexerPower, double scalar) {
    lower_chute_mtr = (double) indexerPower * scalar; //scaling available
}

void flyWheelMoveNoMutex(int flyWheelPower) {
    flywheel_mtr = flyWheelPower;
    flywheel_mtr2 = flyWheelPower;
}




void chuteMove(int chutePower) {
    if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
        ejector_mtr = chutePower;
//        ejector_mtr2 = chutePower;
        lower_chute_mtr = (double) chutePower * .7;
        flywheel_mtr = chutePower;
        flywheel_mtr2 = chutePower;

        ejector_mtrs_mutex.give();
    }
}

void chuteMoveOut(int chutePower) {
    lower_chute_mtr = (double) -chutePower*.7;
    flywheel_mtr = -chutePower;
    flywheel_mtr2 = -chutePower;

}

void chuteShoot(int chutePower) {
    if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
        ejector_mtr = chutePower;
//        ejector_mtr2 = chutePower;
        flywheel_mtr = chutePower;
        flywheel_mtr2 = chutePower;
        lower_chute_mtr = 0; //stop intake
        ejector_mtrs_mutex.give();
    }
}

void chuteIndex(int chutePower) {
  lower_chute_mtr = (double) chutePower*.7;
}

void chuteEject(int chutePower, int indexPower) {
    if (ejector_mtrs_mutex.take(MUTEX_WAIT_SHORT)) {
        ejector_mtr = -chutePower;
//        ejector_mtr2 = -chutePower;
        lower_chute_mtr = (double) indexPower * .7;
        ejector_mtrs_mutex.give();
    }
}

void chuteEjectNoMutex(int ejectorPower, int indexPower) {
    ejector_mtr = -ejectorPower;
//    ejector_mtr2 = -chutePower;
    lower_chute_mtr = (double) indexPower * .7;
    ejector_mtrs_mutex.give();
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


void autoEjectNoBlock(void* param) {
    std::uint32_t now = pros::millis();
    while (true) {
        if (ejectIt) {
            if (ballPos2ColorGet() == 1 && colorModeGet() == EJECT_RED) { //eject red balls
                std::cout << "eject red" << std::endl;
                lessSmartEjectCycle();
            } else if (ballPos2ColorGet() == 2 && colorModeGet() == EJECT_BLUE) { //eject blue balls
                std::cout << "eject blue" << std::endl;
                lessSmartEjectCycle();
            }
        }
    pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }
}



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
    std::cout << "outdexFix: Step 1" << std::endl;
    int elapsed_time = 0;
    int start_time = pros::millis();
    while(ballShootGet() && elapsed_time < 1000) { //downtake until shoot distance doesn't see ball
        std::cout << "outdexFix: Step 2" << std::endl;
        chuteMoveOut(90);
        intakeMove(-10);
        elapsed_time = pros::millis() - start_time;
        pros::delay(20);
    }
    std::cout << "outdexFix: Step 3" << std::endl;
    chuteMoveOut(0);
    intakeMove(0);
    elapsed_time = 0;
    start_time = pros::millis();
    std::cout << "pos 3 state: " << ballPos3Get() << std::endl;
    pros::delay(200);
    while(!ballPos3Get() && elapsed_time < 1000) { //uptake until high optical sensor sees ball
        std::cout << "outdexFix: Step 4" << std::endl;
        chuteIndex(110);
        intakeMove(100);
        elapsed_time = pros::millis() - start_time;
        pros::delay(20);
    }

    chuteIndex(80);
    pros::delay(300);
    chuteIndex(0);

//    intakeMove(50);


    std::cout << "outdexFix: Step 5" << std::endl;
    chuteIndex(0);
    intakeMove(0);
    pros::delay(500);

}

void autoShootOneBall() { //run outdexFix() first
    flywheel_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    flywheel_mtr2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    std::cout << "autoShootOneBall: Step 1" << std::endl;
    int elapsed_time = 0;
    int start_time = pros::millis();
    indexerMove(0,0);
    while ((ballShootGet() || ballPos3Get()) && elapsed_time < 1000) {
        std::cout << "autoShootOneBall: Step 2" << std::endl;
        flyWheelMove(127);
        elapsed_time = pros::millis() - start_time;
        pros::delay(20);
    }
    std::cout << "autoShootOneBall: Step 3" << std::endl;
    pros::delay(200);
    flyWheelMove(0);
}

int right1;
int right1New;
int right2;
int left1;
int left2;

void intakeControl(void* param) {
    //manage side rollers
    std::uint32_t now = pros::millis();
    while(true) {
        left1 = master.get_digital(DIGITAL_L1);
        left2 = master.get_digital(DIGITAL_L2);
        if (intake_mutex.take(MUTEX_WAIT_SHORT)) {
            if (left1) {
                intakeMove(127); //intkae in
            } else if (left2) {
                intakeMove(-90); //open maw
            } else {
                intakeMove(0); //power intake off
            }
            intake_mutex.give();
        }
        pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }
}




    //if the intakes are running in and a ball arrives at a new ballpos, we can assume it came from the below ballpos
    //if the intakes are running out and a ball arrives at a new ballpos, we can assume it came from the above ballpos
      //based on those two logic bits, assign a true false and red blues identifier to each pos
      //use this data to govern the ball movements

//}



void lessSmartEjectCycle(void* param) {
    std::uint32_t now = pros::millis();
    while (true) {
        if ((ballPos2ColorGet() == 1 && colorModeGet() == EJECT_RED) || (ballPos2ColorGet() == 2 && colorModeGet() == EJECT_BLUE)) {
            if (indexer_mutex.take(MUTEX_WAIT_SHORT) && ejector_mutex.take(MUTEX_WAIT_SHORT)) {
                flywheel_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                flywheel_mtr2.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
                flyWheelMove(0);
                std::uint32_t now = pros::millis();
                std::cout << "eject ball cycle" << std::endl;
                int start_time = pros::millis();
                int elapsed_time = 0;
                double scalar = 1;
                std::cout << "eject BEGIN" << std::endl;
                while (!ballLeaveGet() && elapsed_time < 1000) {
                    std::cout << "slow" << std::endl;
                    indexerMove(100, 1);
                    ejectorMove(-100);
                    std::cout << elapsed_time << std::endl;
//                    if (ballPos2ColorGet() == 2 && colorModeGet() == EJECT_RED) { //don't eject the blue coming in next
//                        std::cout << "stop extra blue eject" << std::endl;
//                        indexerMoveNoMutex(0, 0.0);
//                    } else if (ballPos2ColorGet() == 1 &&
//                               colorModeGet() == EJECT_BLUE) { //don't eject the red coming in next
//                        std::cout << "stop extra red eject" << std::endl;
//                        indexerMoveNoMutex(0, 0.0);
//                    }
                    elapsed_time = pros::millis() - start_time;
                    pros::delay(20);
                }
                indexerMoveNoMutex(0, 1);
                ejectorMoveNoMutex(0);
                std::cout << "next" << std::endl;

                ejector_mutex.give();
                indexer_mutex.give();
            }
        }
        pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }

}



void chuteLessSmartControl(void* param) {
    std::uint32_t now = pros::millis();
    int startTime = 0;
    bool doubleClick = false;
    bool flyWheelBlock = false;


    while(true) {
        lower_chute_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        ejector_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheel_mtr.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        flywheel_mtr2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        std::uint32_t now = pros::millis();
        left1 = master.get_digital(DIGITAL_L1);
        left2 = master.get_digital(DIGITAL_L2);
        right1 = master.get_digital(DIGITAL_R1);
        right1New = master.get_digital_new_press(DIGITAL_R1);
        right2 = master.get_digital(DIGITAL_R2);

        if (chute_mutex.take(MUTEX_WAIT_SHORT)) {
            //doubleClick Logic
            if (right1New && now - startTime < 1000) { // if double click right1
                doubleClick = true;
            } else if (right1New) {
                startTime = now;
                doubleClick = false;
            }

//            //eject logic
//            if (ballPos2ColorGet() == 1 && colorModeGet() == EJECT_RED) { //eject red balls
//                std::cout << "eject red" << std::endl;
//                lessSmartEjectCycle();
//            } else if (ballPos2ColorGet() == 2 && colorModeGet() == EJECT_BLUE) { //eject blue balls
//                std::cout << "eject blue" << std::endl;
//                lessSmartEjectCycle();
//            }

            //button logic
             if (left1 && !right1) { //index balls
                if (indexer_mutex.take(MUTEX_WAIT_SHORT)) {
                    if (flyWheelBlock) {
                        flyWheelMove(0);
                    }
                    flyWheelBlock = false;
//                    std::cout << "index balls" << std::endl;
                    std::cout << "speed" << std::endl;
                    flyWheelMove(-20);
                    indexerMove(127, .7);
                    indexer_mutex.give();
                }
            } else if (right1) {
                 flyWheelBlock = true;
                if (doubleClick) { // if double click right1
                    if (indexer_mutex.take(MUTEX_WAIT_SHORT)) {
                        indexerMove(127, .7);
                        flyWheelMove(127);
                        indexer_mutex.give();
                    }
                } else {
                    flyWheelMove(127);
                    indexerMove(0, .7);
                }
            } else if (right2) {
                 if (indexer_mutex.take(MUTEX_WAIT_SHORT)) {
                     indexerMove(-127, .7);
                     flyWheelMove(-127);
                     indexer_mutex.give();
                 }
            } else {
                 if (indexer_mutex.take(MUTEX_WAIT_SHORT)) {
                     indexerMove(0, .7);
                     indexer_mutex.give();
                 }
                 flyWheelMove(0);
             }
            chute_mutex.give();
        }
        pros::Task::delay_until(&now, TASK_DELAY_NORMAL);
    }
}




void chuteControlTaskInit() {
    pros::Task eject_task(lessSmartEjectCycle, (void*)"EJECT_TASK");
    pros::Task intake_task(intakeControl,(void*)"INTAKE_TASK");
    pros::Task chute_task(chuteLessSmartControl,(void*)"CHUTE_TASK");
}

void chuteAutoTaskInit() {
    pros::Task chute_auto_task(autoEjectNoBlock,(void*)"CHUTE_AUTO_TASK");
}

