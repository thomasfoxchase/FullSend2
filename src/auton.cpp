#include "main.h"
#include "base.h"
#include "telemetry.h"
#include "chute.h"
#include "definitions.h"
#include "auton.h"
#include "motors.h"


void autoTester() {
    autoShootOneBall();
}


void blueAuton() {
    beginTimer(true);
    colorModeSet(BLUE);
    intakeMove(127);
    chuteIndex(90);
    basePIDMove(-1.25, 5000); //pick up first ball
    basePIDTurn(42, 5000); //turn to goal
    basePIDMove(2.0, 5000); //pick up first ball
//    pros::delay(500);
    chuteIndex(0);
    basePIDMove(-0.5, 500); //pick up first ball
    basePIDMove(0.5, 500); //pick up first ball
    autoShootOneBall();
    chuteIndex(90);
////autoShootUntil(BLUE, 2000);
    basePIDMove(-1, 5000);
    basePIDTurn(120, 5000);
    chuteIndex(50);
//    intakeMove(0);
    setEjectIt(true);
    basePIDMove(3.7, 2000);
//    basePIDMove(-1, 2000);
//    basePIDMove(1, 2000);
//    basePIDMove(-1, 2000);
//    basePIDMove(1.2, 2000);
    basePIDTurn(-30, 500);
//    basePIDTurn(-10, 5000);
//    chuteMove(-50); //reset blue ball if coming out top
//    pros::delay(300);
//    chuteIndex(100);
    autoShootOneBall();
    chuteIndex(90);
    pros::delay(500);
    chuteIndex(0);
    basePIDMove(-2, 5000);
    setEjectIt(false);
    basePIDTurn(-55, 5000);
    basePIDMove(-3.5, 3000);
    basePIDTurn(90, 5000);
    setEjectIt(true);
    intakeMove(127);
    basePIDMove(1.5, 2000);
    chuteMove(-50); //reset blue ball if coming out top
    pros::delay(300);
    chuteIndex(100);
    pros::delay(1000);
    chuteIndex(0);
    intakeMove(0);
    autoShootOneBall();
    pros::delay(1000);
    autoShootOneBall();
    setEjectIt(false);
    while (true) {

        if(timeGet() > 4400) {
            basePIDMove(-3, 2000); // back of at end of auto
        }
    }
}

void blueAuton2() {
    beginTimer(true);
//    colorModeSet(BLUE);
    intakeMove( 127);
    chuteIndex(90);
    basePIDMove(-1.25, 5000); //pick up first ball
    basePIDTurn(42, 5000); //turn to goal
    basePIDMove(2.0, 5000); //pick up first ball
//    pros::delay(500);
    chuteIndex(0);
    basePIDMove(-0.5, 500); //pick up first ball
    basePIDMove(0.5, 500); //pick up first ball
    autoShootOneBall();
    intakeMove(0);
    chuteIndex(90);
////autoShootUntil(BLUE, 2000);
    basePIDMove(-1, 5000);
    intakeMove(127);
    basePIDTurn(100, 5000);
    chuteIndex(50);
    intakeMove(0);
    basePIDMove(1.7, 2000);
    basePIDTurn(30, 5000);
    basePIDMove(3, 1500);
//    basePIDMove(-1, 500);
//    basePIDMove(3, 800);
//    basePIDMove(-1, 500);
//    basePIDMove(3, 800);
    basePIDTurn(-30, 500);
//    basePIDTurn(-10, 5000);
//    chuteMove(-50); //reset blue ball if coming out top
//    pros::delay(300);
//    chuteIndex(100);
//    pros::delay(500);
    outdexFix();
    autoShootOneBall();
    setEjectIt(true);
    chuteIndex(90); //eject red
//    chuteIndex(90);
//    pros::delay(500);
//    chuteIndex(0);
    basePIDMove(-2, 5000);
    setEjectIt(false);
    basePIDTurn(110, 5000);
    basePIDMove(4.1, 3000);

    basePIDTurn(-90, 5000);
    setEjectIt(true);
    chuteIndex(90);
    intakeMove(127);
    basePIDMove(2, 2000);
    chuteIndex(0);
    intakeMove(0);
    autoShootOneBall();
    pros::delay(1000);
    autoShootOneBall();
    setEjectIt(false);
//    while (true) {
//
//        if(timeGet() > 4400) {
//            basePIDMove(-3, 2000); // back of at end of auto
//        }
//    }
}

void blueAuton3() {
    tareInertial();
    beginTimer(true);
    colorModeSet(BLUE);
    intakeMove(127);
    chuteIndex(90);
    basePIDMoveHeading(-1.25, 0, 0.2, 5000); //pick up first ball
    basePIDTurnAbsolute(42, 5000); //turn to goal
    basePIDMoveHeading(2.0, 42, 0.2, 5000); //pick up first ball
//    pros::delay(500);
    chuteIndex(0);
    basePIDMoveHeading(-0.5, 42, 0.2, 500); //pick up first ball
    basePIDMoveHeading(0.5, 42, 0.2, 500); //pick up first ball
    autoShootOneBall();
    intakeMove(0);
    chuteIndex(90);
////autoShootUntil(BLUE, 2000);
    basePIDMoveHeading(-1, 42, 0.2, 5000);
    intakeMove(127);
    basePIDTurnAbsolute(142, 5000);
    chuteIndex(50);
    intakeMove(0);
    basePIDMoveHeading(1.7, 142, 0.2, 2000);
    basePIDTurnAbsolute(172, 5000);
    basePIDMoveHeading(3, 162, 0.2, 1000);
//    basePIDMove(-1, 500);
//    basePIDMove(3, 800);
//    basePIDMove(-1, 500);
//    basePIDMove(3, 800);
    basePIDTurn(-30, 500);
//    basePIDTurn(-10, 5000);
//    chuteMove(-50); //reset blue ball if coming out top
//    pros::delay(300);
//    chuteIndex(100);
//    pros::delay(500);
    outdexFix();
    autoShootOneBall();
    setEjectIt(true);
    chuteIndex(90); //eject red
//    chuteIndex(90);
//    pros::delay(500);
//    chuteIndex(0);
    intakeMove(40);
    basePIDMoveHeading(-2, 160, 0.2, 5000);
    intakeMove(0);
    setEjectIt(false);
    basePIDTurnAbsolute(265, 5000);
//    basePIDMoveHeading(-4, 270, 0.2, 2000);
//    tareInertial();
    basePIDMoveHeading(4, 270, 0.2, 3000);

    basePIDTurnAbsolute(180, 5000);
    setEjectIt(true);
    chuteIndex(90);
    intakeMove(127);
    basePIDMoveHeading(2, 180, 0.2, 2000);
    chuteIndex(0);
    intakeMove(0);
    autoShootOneBall();
    pros::delay(1000);
    autoShootOneBall();
    setEjectIt(false);
//    while (true) {
//
//        if(timeGet() > 4400) {
//            basePIDMove(-3, 2000); // back of at end of auto
//        }
//    }
}

void blueAuton4() {
    tareInertial();
    beginTimer(true);
    colorModeSet(BLUE);
    setEjectIt(false);

    chuteMoveOut(127); //deploy
    pros::delay(200);
    chuteMoveOut(0);

    intakeMove(127);
    chuteIndex(90);
    basePIDMoveHeading(-1.25, 0, 0.2, 5000); //pick up first ball
    basePIDTurnAbsolute(42, 5000); //turn to goal
    basePIDMoveHeading(3.0, 42, 0.2, 1500); //pick up first ball
//    pros::delay(500);
    chuteIndex(0);
    basePIDMoveHeading(-1, 42, 0.2, 400); //pick up first ball
    basePIDMoveHeading(1, 42, 0.2, 400); //pick up first ball
    outdexFix();
    autoShootOneBall();
    intakeMove(10);
    chuteIndex(127);
////autoShootUntil(BLUE, 2000);

    basePIDMoveHeading(-1, 42, 0.2, 5000);
    intakeMove(-127);
    pros::delay(500);
    basePIDTurnAbsolute(142, 5000);
    basePIDMoveHeading(1.7, 142, 0.2, 2000);
    basePIDTurnAbsolute(172, 5000);
    intakeMove(127);
    basePIDMoveHeading(3.5, 170, 0.2, 2000);
//    basePIDMove(-1, 800);
//    intakeMove(127);
//    basePIDMove(3, 800);
//    basePIDMove(-1, 800);
//    basePIDMove(3, 800);
    intakeMove(0);
    basePIDTurn(-30, 500);



//    basePIDMoveHeading(-2.2, 42, 0.2, 5000);
//    intakeMove(-127);
//    basePIDTurnAbsolute(137, 5000);
//    intakeMove(127);
//    setEjectIt(true);
////    intakeMove(-70);
//    basePIDMoveHeading(2.8, 137, 0.2, 2000); //drive into second goal
////    intakeMove(127); //intkae balls

    outdexFix();
    autoShootOneBall();
    intakeMove(0);
    chuteIndex(90);

    basePIDMoveHeading(-2, 137, 0.2, 5000);
//    setEjectIt(false);
    basePIDTurnAbsolute(270, 5000);
    basePIDMoveHeading(-4, 270, 0.2, 2000);
    tareInertial();
    basePIDMoveHeading(5.3, 0, 0.2, 3000);
    basePIDTurnAbsolute(-90, 5000);
//    setEjectIt(true);
    chuteIndex(90);
    intakeMove(127);
    basePIDMoveHeading(2.7, -90, 0.2, 2000);
    chuteIndex(0);
    intakeMove(0);
    outdexFix();
    autoShootOneBall();
//    pros::delay(1000);
//    autoShootOneBall();
//    setEjectIt(false);
    while (true) {
        if(timeGet() > 44200) {
            basePIDMove(-2, 2000); // back of at end of auto
            break;
        }
        pros::delay(20);
    }
}


void newAuton() {
    colorModeSet(EJECT_BLUE);
    tareInertial();
    beginTimer(true);
//    colorModeSet(BLUE);
    setEjectIt(false);

    flyWheelMove(-127); //deploy
    pros::delay(200);
    flyWheelMove(0);

    intakeMove(127);
    indexerMove(90, 1);
    basePIDMoveHeading(-1.25, 0, 0.2, 5000); //pick up first ball
    basePIDTurnAbsolute(42, 5000); //turn to goal
    basePIDMoveHeading(3.0, 42, 0.2, 1500); //pick up first ball
//    pros::delay(500);
    indexerMove(0,0);
//    basePIDMoveHeading(-1, 42, 0.2, 400); //pick up first ball
//    basePIDMoveHeading(1, 42, 0.2, 400); //pick up first ball
//    outdexFix();
    autoShootOneBall();
    pros::delay(500);
    intakeMove(0);
//    intakeMove(-50);
    indexerMove(127,1);
    basePIDMoveHeading(-1, 42, 0.2, 5000);
    intakeMove(0);
    indexerMove(10,1);
    basePIDTurnAbsolute(142, 5000);
    basePIDMoveHeading(1.7, 142, 0.2, 2000);
    basePIDTurnAbsolute(172, 5000);
    intakeMove(70);
    basePIDMoveHeading(3.5, 170, 0.2, 2000);
//    basePIDMoveHeading(-1, 170, 0.2, 1500);
//    intakeMove(0);
//    baseMove(60,60);
//    pros::delay(500);
//    baseMove(0,0);
//    pros::delay(500);
//    basePIDMoveHeading(-1, 170, 0.2, 1500);
//    baseMove(60,60);
//    pros::delay(500);
//    baseMove(0,0);




//    basePIDMoveHeading(-2, 170, 0.2, 1500);
//    intakeMove(0);
//    basePIDMoveHeading(3, 170, 0.2, 1500);
//    basePIDMoveHeading(-2, 170, 0.2, 1500);
//    basePIDMoveHeading(3, 170, 0.2, 1500);
    basePIDTurn(-30, 500);

//    outdexFix();
    autoShootOneBall();
    pros::delay(500);
    intakeMove(-50);
    basePIDMoveHeading(-2, 137, 0.2, 5000);
//    setEjectIt(falsduejube);
    indexerMove(90,1);
    intakeMove(0);
    basePIDTurnAbsolute(270, 5000);
    indexerMove(10,1);
    basePIDMoveHeading(-3, 270, 0.2, 1700);
    tareInertial();
    basePIDMoveHeading(5.3, 0, 0.2, 3000);
    basePIDTurnAbsolute(-90, 5000);
//    setEjectIt(true);
    indexerMove(90,1);
    intakeMove(-70);
    basePIDMoveHeading(2.8, -90, 0.2, 2000);
    indexerMove(0,0);
    intakeMove(0);
//    outdexFix();
    autoShootOneBall();
//    basePIDMove(-2, 2000); // back of at end of auto

    while (true) {
        if(timeGet() > 44200) {
            basePIDMove(-2, 2000); // back of at end of auto
            break;
        }
        pros::delay(20);
    }
}



void redAuton() {
//    colorModeSet(RED);

//    outdexFix();
//    autoShootOneBall();

    beginTimer(true);
    while(true) {
        pros::delay(500);
        while (true) {
            std::cout << "Time:" << timeGet() << std::endl;
            if(timeGet() > 10000) {
                std::cout << "Do it Time:" << timeGet() << std::endl;
//                basePIDMove(-2, 2000); // back of at end of auto
                break;
            }
            pros::delay(20);
        }
    }

}



