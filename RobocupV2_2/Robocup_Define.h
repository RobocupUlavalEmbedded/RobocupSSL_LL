/*
 * Robocup_Define.h
 *
 *  Created on: 2013-10-16
 *      Author: Mathieu Garon
 */

#ifndef ROBOCUP_DEFINE_H_
#define ROBOCUP_DEFINE_H_

#include <stdio.h>
#include <file.h>

#include <stdbool.h>
typedef unsigned char uint8_t;
typedef _Bool bool_t;


/*------------------------------------------------------------------------------------------------
 * 	 SYS/BIOS's files
 *------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Swi.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>

/*------------------------------------------------------------------------------------------------
 * 	 Picollo F28027's driver files
 *------------------------------------------------------------------------------------------------*/
#include "f2802x_common/include/gpio.h"
#include "f2802x_common/include/adc.h"
#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/flash.h"
#include "f2802x_common/include/pie.h"
#include "f2802x_common/include/pll.h"
#include "f2802x_common/include/wdog.h"
#include "f2802x_common/include/pwm.h"
#include "f2802x_common/include/spi.h"
#include "f2802x_common/include/sci.h"
#include "f2802x_common/include/sci_io.h"


/*------------------------------------------------------------------------------------------------
 * 	 Robots constant
 *------------------------------------------------------------------------------------------------*/
#define WHEEL_DIAMETER 0.064//0.775
#define RRTIME 0.01		//RoundRobin call time in S
#define ENCODER_PPR 1020			//encoder pulses
#define ONE_ENCODER_PPR 0.0009804	// 1/ENCODER_PPR = 1/1020
#define PI _IQ(3.1415926535897932384626433832795)

#define MIN_SPEED _IQ(0.2)

/*--------------------------------------------------------------------------------------------------
 * Genral constant
 *--------------------------------------------------------------------------------------------------*/

#define PLAYER_BUFFER_SIZE 15
/*------------------------------------------------------------------------------------------------
 * 	 IQ math
 *------------------------------------------------------------------------------------------------*/
/*************************
 * 	Q	       precision
 * 	23 => -256 to 255.999 999 981
 *
 *************************/
#define GLOBAL_Q 23  	//global precision
#include <IQmathLib.h>  //Virtual FPU
void robotParam_init();


// Work around for code that might accidently use uint8_t
//typedef unsigned char uint8_t;

typedef short             int16;
//typedef unsigned short    Uint16;

/****************************************************************************
 * 								RF MODULE
 ****************************************************************************/

typedef struct nRF24L01_Handle {
	GPIO_Number_e structCePin;
	GPIO_Number_e structCSNPin;
	uint16_t Reg[10];  //generic registers
	uint8_t TXPayload[PLAYER_BUFFER_SIZE];
	uint8_t RXPayload[PLAYER_BUFFER_SIZE];
	uint16_t TXADDR[5];
	uint16_t RXADDR[5];
	uint8_t RXCount;
	uint8_t RobotID;
}nRF24L01_Handle;

/****************************************************************************
 * 								QUADRATURE DECODER
 ****************************************************************************/

typedef enum{
	CS_0,
	CS_1,
	CS_2,
	CS_3,
	CS_4,
	CS_5,
	CS_6,
	CS_7
} chip_select;

typedef struct quad_Handle {
	chip_select structCSNPin;
	int16_t Count0;
	int16_t Count1;
	_iq wheelVelocity[2];
}quad_Handle;

/****************************************************************************
 * 								PARAMETER
 ****************************************************************************/
typedef union{
	uint8_t bytes[2];
	float floating;
}converter;

typedef struct robot_param{
	_iq wheelRadius;
	uint32_t encoderPPR;			//pulse per round
	_iq roundRobinTime;
	_iq speedFactor;
	converter XVelocityCommand;
	converter YVelocityCommand;
	converter ThetaVelocityCommand;
}robot_param;

/****************************************************************************
 * 								GYRO
 ****************************************************************************/

typedef struct L3GD20_Handle {
	chip_select SCpin;
	uint16_t outZ;
}L3GD20_Handle;

/****************************************************************************
 * 								IMU
 ****************************************************************************/

typedef struct LSM9DS0_Handle {
	chip_select GyroSCpin;
	chip_select AccSCpin;
}LSM9DS0_Handle;


/****************************************************************************
 * 								DEMUX
 ****************************************************************************/

typedef struct Demux_Handle {
	GPIO_Number_e a0;
	GPIO_Number_e a1;
	GPIO_Number_e a2;
	chip_select notConnect_pin;
}Demux_Handle;

/****************************************************************************
 * 								ARDUINO
 ****************************************************************************/

typedef struct arduino_Handle {
	chip_select CSPin;
}arduino_Handle;

/****************************************************************************
 * 								MOTOR
 ****************************************************************************/

typedef struct DCMotor_Handle {
	PWM_Handle PwmHandler;
	GPIO_Number_e DirectionPin;
	bool aPwm;
}DCMotor_Handle;

/****************************************************************************
 * 								PID
 ****************************************************************************/

#define PID_P _IQ(1)
#define PID_I _IQ(0)
//#define PID_I _IQ(0.005)
#define PID_D _IQ(0.000)

typedef struct {
		_iq Ref; // Input: reference set-point
		_iq Fbk; // Input: feedback
		_iq Out; // Output: controller output
		_iq c1; // Internal: derivative filter coefficient
		_iq c2; // Internal: derivative filter coefficient
} PID_GRANDO_TERMINALS;
typedef struct {
		_iq Kr; // Parameter: proportional reference weighting
		_iq Kp; // Parameter: proportional loop gain
		_iq Ki; // Parameter: integral gain
		_iq Kd; // Parameter: derivative gain
		_iq Km; // Parameter: derivative reference weighting
		_iq Umax; // Parameter: upper saturation limit
		_iq Umin; // Parameter: lower saturation limit
} PID_GRANDO_PARAMETERS;
typedef struct {
		_iq up; // Data: proportional term
		_iq ui; // Data: integral term
		_iq ud; // Data: derivative term
		_iq v1; // Data: pre-saturated controller output
		_iq i1; // Data: integrator storage: ui(k-1)
		_iq d1; // Data: differentiator storage: ud(k-1)
		_iq d2; // Data: differentiator storage: d2(k-1)
		_iq w1; // Data: saturation record: [u(k-1) - v(k-1)]
} PID_GRANDO_DATA;
typedef struct {
		PID_GRANDO_TERMINALS term;
		PID_GRANDO_PARAMETERS param;
		PID_GRANDO_DATA data;
} PID_Handle;

/****************************************************************************
 * 								ROBOT
 ****************************************************************************/

typedef struct Robot_Handle {
	// C2000
	SCI_Handle HandleSCI;
	CLK_Handle HandleCLK;
	GPIO_Handle HandleGPIO;
	SPI_Handle HandleSPI;
	FLASH_Handle HandleFlash;
    CPU_Handle HandleCpu;
    PLL_Handle HandlePll;
    WDOG_Handle HandleWDog;
    ADC_Handle HandleADC;
    PIE_Handle HandlePIE;

    // Robot
    Demux_Handle HandleDemux;
    nRF24L01_Handle HandleRF;
    quad_Handle HandleQuad[2];
    robot_param robotParam;
    L3GD20_Handle HandleGyro;
    LSM9DS0_Handle HandleImu;
    arduino_Handle HandleArduino;
    PWM_Handle HandlePwm1;
    PWM_Handle HandlePwm2;
    PWM_Handle HandlePwm3;
    DCMotor_Handle HandleMotor[4];
    PID_Handle HandlePid[4];
} Robot_Handle;

extern Robot_Handle HandleRobot;

#define HandleDemux  HandleRobot.HandleDemux
#define HandleRF  HandleRobot.HandleRF
#define HandleGyro  HandleRobot.HandleGyro
#define HandleImu HandleRobot.HandleImu
#define HandleArduino  HandleRobot.HandleArduino

//extern Robot_Handle HandleRobot;

#endif /* ROBOCUP_DEFINE_H_ */
