/**********************************************************************************************
 * Arduino PID Library - Version 1.2.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under the MIT License
 **********************************************************************************************/

#include <inttypes.h>

#include "pid.h"
#include "pwm.h"
#include "pulsectl.h"

double kp=2.5;        // * (P)roportional Tuning Parameter
double ki=0.0002;     // * (I)ntegral Tuning Parameter
double kd=100;        // * (D)erivative Tuning Parameter

double outputSum, lastInput;

double outMax = 255;
double outMin = -outMax;
bool pOnE = true;

/* Initialize()****************************************************************
 *  does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/

void pid_init(int16_t position){
    outputSum = 0;
    lastInput = position;
}

/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/

int16_t pid_position_to_pwm(int16_t position){
    /*Compute all the working error variables*/
    double input = position;
    double error = reg_seek_position - input;
    double dInput = (input - lastInput);

    //prevent outputSum windup when PID calculation isn't nessesary
    if(!reg_pwm_pid_enabled){
        lastInput = input;
        outputSum = 0;
        return 0;
    }
    outputSum+= (ki * error);

    /*Add Proportional on Measurement, if P_ON_M is specified*/
    if(!pOnE) outputSum-= kp * dInput;

    if(outputSum > outMax) outputSum= outMax;
    else if(outputSum < outMin) outputSum= outMin;

    /*Add Proportional on Error, if P_ON_E is specified*/
    double output;
    if(pOnE) output = kp * error;
    else output = 0;

    /*Compute Rest of PID Output*/
    output += outputSum - kd * dInput;

    if(output > outMax) output = outMax;
    else if(output < outMin) output = outMin;

    /*Remember some variables for next time*/
    lastInput = input;

    return output;


}
