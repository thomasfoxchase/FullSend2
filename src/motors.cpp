#include "motors.h"
#include "definitions.h"

void motorBrake(pros::Motor motor, pros::Motor motor2) {
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    motor2.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    pros::delay(20);
    motor.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
    motor2.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
}

//base
pros::Motor right_front_mtr(FRONT_RIGHT_DRIVE_PORT);
pros::Motor left_front_mtr(FRONT_LEFT_DRIVE_PORT, REVERSE);
pros::Motor right_middle_mtr(MIDDLE_RIGHT_DRIVE_PORT, REVERSE);
pros::Motor left_middle_mtr(MIDDLE_LEFT_DRIVE_PORT);
pros::Motor right_back_mtr(BACK_RIGHT_DRIVE_PORT);
pros::Motor left_back_mtr(BACK_LEFT_DRIVE_PORT, REVERSE);

//chute
pros::Motor lower_chute_mtr(LOWER_CHUTE_PORT, REVERSE);

//intake
pros::Motor left_intake_mtr(LEFT_INTAKE_PORT);
pros::Motor right_intake_mtr(RIGHT_INTAKE_PORT, REVERSE);

//ejector
pros::Motor ejector_mtr(EJECTOR_PORT);
//pros::Motor ejector_mtr2(EJECTOR2_PORT);

//flywheel
pros::Motor flywheel_mtr(FLYWHEEL_PORT);
pros::Motor flywheel_mtr2(FLYWHEEL2_PORT, REVERSE);




bool chuteDirectionGet() {
    return lower_chute_mtr.get_direction();
}

pros::Motor upper_chute_mtr(UPPER_CHUTE_PORT);
