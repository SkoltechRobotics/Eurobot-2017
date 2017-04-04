#include "Manipulators.h"
#include "Dynamixel_control.h"
#include "Regulator.h"
#include "Board.h"
#include "math.h"


double timeofred;
char color, color_check[8];

float r,b,R,B;
float whole_angle, values[10], angle_encoder, angle_enc_real, whole_starting_angle, whole_angle_prev, angle_before_movement;
bool direction = true;

extern int numberofrot;

//values[0] = 0;
//values[1] = 0;
//values[2] = 0;
//values[3] = 0;
//values[4] = 0;
//values[5] = 0;
//values[6] = 0;
//values[7] = 0;
//values[8] = 0;
//values[9] = 0;


void softDelay(__IO unsigned long int ticks)
{
    for(; ticks > 0; ticks--);
}


bool goInsideWithSuckingManipulator(){

    set_pin(INPUT1_CONTROL);
    reset_pin(INPUT2_CONTROL);

    while(!pin_val(UPPER_SWITCH));


    reset_pin(INPUT1_CONTROL);
}

bool goOutsideWithSuckingManipulator(){

    set_pin(INPUT2_CONTROL); //set and reset pin do not work
    reset_pin(INPUT1_CONTROL);


    while(!pin_val(DOWN_SWITCH));


    reset_pin(INPUT2_CONTROL);
}
/*  this commented function is wrong, so don't check it and don't use it
void GetDataForManipulator(void)
{
    int i = 3000000;
    CubesCatcherAngle = adcData[(char)CUBES_CATCHER_ADC - 1] / 36 * 3.3;//*360/3.3

    while(--i > 0);

    float sum=0;
    for(i=0;i<9;i++){
        values[i] = values[i+1];
        sum+=values[i];
    }
    sum += values[9];
    sum/=10;

    if(( previous_status < 380  &&  previous_status >= 270 ) && ( sum < 90  &&  sum >= 0 ))
         number_full_rotations++;

    if((previous_status >= 0  &&  previous_status < 90 ) && ( sum < 380 && sum >= 270 ))
        number_full_rotations--;

//    if(fabs(CubesCatcherAngle - previous_status)>100)
//        number_full_rotations--;



    values[9] = CubesCatcherAngle;

    whole_angle = number_full_rotations*360 + CubesCatcherAngle;

    previous_status = sum;
}*/

char getCurrentColor(){
    int z = 1000;
    while(z>0){
        z--;
        reset_pin(EXTI2_PIN);//red, S3
        int j,i = 15000;
            for(; i> 0; i--);
        r = timeofred;
        R = 10000./(r);
        i = 15000;
            for(; i> 0; i--);

        set_pin(EXTI2_PIN);//blue

        i = 15000;
            for(; i> 0; i--);
        b = timeofred;
        B = 10000./(b);
////
        if(R >= B){
            if(R/B>2)color = 'Y';
            else color = 'W';
        }
        else{
            if(B<200 && B/R>1.1) color = 'B';
            else color = 'W';
        }
        softDelay(500);
        for(j=0;j<7;j++)color_check[j] = color_check[j+1]; //filter
        color_check[7] = color;

        for(j=0;j<8;j++)if(color_check[i] != color){color='n';break;}

        if(color!='n') break;
    }

    return color;
}


float getCurrentEncoderAngle(void){

    angle_encoder = adcData[(char)CUBES_CATCHER_ADC - 1] *360 / 4096;
    angle_enc_real = 32*pow(angle_encoder, 0.41) - 27/360 * angle_encoder + 30 - 65*exp(-pow((angle_encoder-55)/60, 2)) + 25*exp(-pow((angle_encoder-117)/33, 2));
    whole_angle = numberofrot * 360 + angle_enc_real;
    return angle_enc_real;
}

void setCurrentAngleAsBeginning(void){
    whole_angle = getCurrentEncoderAngle();
    whole_starting_angle = getCurrentEncoderAngle();
}

void setPositionOfCylinderCarrier(float desiredAngle){

    angle_encoder = adcData[(char)CUBES_CATCHER_ADC - 1] *360 / 4096;
    angle_enc_real = 32*pow(angle_encoder, 0.41) - 27/360 * angle_encoder + 30 - 65*exp(-pow((angle_encoder-55)/60, 2)) + 25*exp(-pow((angle_encoder-117)/33, 2));
    whole_angle = numberofrot * 360 + angle_enc_real;
    angle_before_movement = whole_angle;

    if(desiredAngle > whole_angle){

        whole_angle_prev = whole_angle - 10;

        while(fabs(desiredAngle - whole_angle) > 15 && (desiredAngle > whole_angle)){

            angle_encoder = adcData[(char)CUBES_CATCHER_ADC - 1] *360 / 4096;
            angle_enc_real = 32*pow(angle_encoder, 0.41) - 27/360 * angle_encoder + 30 - 65*exp(-pow((angle_encoder-55)/60, 2)) + 25*exp(-pow((angle_encoder-117)/33, 2));
            whole_angle = numberofrot * 360 + angle_enc_real;

            setServoMovingSpeed(3, (uint16_t)(710), 0x0000);//CCW
            setServoMovingSpeed(2, (uint16_t)(850 + 1024), 0x0400);//CW

            if(((desiredAngle - whole_angle) > (desiredAngle - whole_angle_prev)) && fabs(angle_before_movement - whole_angle) > fabs(desiredAngle - angle_before_movement)){//-10 at the end
                setServoMovingSpeed(3, (uint16_t)0, 0x0000);
                setServoMovingSpeed(2, (uint16_t)0, 0x0000);
//                whole_angle += 2*fabs(angle_before_movement - whole_angle);
                break;
            }
            whole_angle_prev = whole_angle - 20;
        }
    }

    else if(desiredAngle < whole_angle){

        whole_angle_prev = whole_angle + 10;

        while(fabs(desiredAngle - whole_angle) > 15 && (desiredAngle < whole_angle)){

            angle_encoder = adcData[(char)CUBES_CATCHER_ADC - 1] *360 / 4096;
            angle_enc_real = 32*pow(angle_encoder, 0.41) - 27/360 * angle_encoder + 30 - 65*exp(-pow((angle_encoder-55)/60, 2)) + 25*exp(-pow((angle_encoder-117)/33, 2));
            whole_angle = numberofrot * 360 + angle_enc_real;

            setServoMovingSpeed(3, (uint16_t)(780 + 1024), 0x0400);//CW
            setServoMovingSpeed(2, (uint16_t)(810), 0x0000);//CCW

            if((fabs(desiredAngle - whole_angle) > fabs(desiredAngle - whole_angle_prev)) && fabs(angle_before_movement - whole_angle) > fabs(desiredAngle - angle_before_movement)){//-10 at the end
                setServoMovingSpeed(3, (uint16_t)0, 0x0000);
                setServoMovingSpeed(2, (uint16_t)0, 0x0000);
//                whole_angle -= 2*fabs(angle_before_movement - whole_angle);
                break;
            }

            whole_angle_prev = whole_angle + 20;
        }
    }
    setServoMovingSpeed(2, (uint16_t)0, 0x0000);
    setServoMovingSpeed(3, (uint16_t)0, 0x0000);
}


void increaseByGivenAngle(float givenAngle){
    setPositionOfCylinderCarrier(whole_angle + givenAngle);
}



void decreaseByGivenAngle(float givenAngle){
    setPositionOfCylinderCarrier(whole_angle - givenAngle);
}


void servo_elevate_in()
{
    setServoAngle((uint8_t)SERVO_ELEVATE, (uint16_t) SERVO_ELEVATE_IN);
}

void servo_elevate_out()
{
    setServoAngle((uint8_t)SERVO_ELEVATE, (uint16_t) SERVO_ELEVATE_OUT);
}

void servo_rotate_90()
{
    setServoAngle((uint8_t)SERVO_ROTATE, (uint16_t) SERVO_ROTATE_90);
}

void servo_rotate_180()
{
    setServoAngle((uint8_t)SERVO_ROTATE, (uint16_t) SERVO_ROTATE_180);
}
float CubesCatcherAngle = 0;
float prevCubesCatcherAngle = 0;
float diff;
float arCubesCatcherAngle[10];

float encodermagner(float prevencodermagner){

        arCubesCatcherAngle[9] = adcData[(char)CUBES_CATCHER_ADC - 1] *360 / 4096;//*360/3.3
        int i = 0;
        float smoothed = 0;
        for (i;i<9;i++) {
                arCubesCatcherAngle[i] = arCubesCatcherAngle[i+1];
                smoothed+= arCubesCatcherAngle[i];
        }
        smoothed /= 10;


        if ((arCubesCatcherAngle[0] - prevCubesCatcherAngle) > 100) {
                diff = 1;
                //softDelay(100000);
        }
        else if ((arCubesCatcherAngle[0]- prevCubesCatcherAngle) < -100) {
                diff = -1;
                //softDelay(100000);
        }
        else diff = 0;
        prevCubesCatcherAngle = smoothed;
        if ((diff != 0)  && (prevencodermagner !=0)){
            diff =2;}
        return diff;
}

float diff;
float arCubesCatcherAngle[10];



bool switchOnPneumo()
{
//    set_pin(PIN1_12V);
    set_pin(PIN2_12V);
//    set_pin(PIN3_12V);
//    set_pin(PIN4_12V);
//    set_pin(PIN5_12V);
//    set_pin(PIN6_12V);
    return 0;
}

bool switchOffPneumo()
{
//    reset_pin(PIN1_12V);
    reset_pin(PIN2_12V);
//    reset_pin(PIN3_12V);
//    reset_pin(PIN4_12V);
//    reset_pin(PIN5_12V);
//    reset_pin(PIN6_12V);
    return 0;
}


///////////////////////////////////////////////////////////////
