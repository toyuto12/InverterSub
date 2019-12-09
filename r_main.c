/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G14 V2.05.03.02 [06 Nov 2018]
* Device(s)    : R5F104BF
* Tool-Chain   : CCRL
* Description  : This file implements main function.
* Creation Date: 2019/12/09
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_dac.h"
#include "r_cg_timer.h"
#include "r_cg_it.h"
#include "r_cg_dtc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */

stReceiveData gRev;
uint8_t gInterval;
uint8_t gIsReceived;
uint8_t gCapEnable;
uint16_t cyc,adTmp[16];
uint16_t sTimeOut;

uint32_t rpmTmp[4];
uint8_t rpmCyc;


#define SOFT_VER	0
#define SOFT_REV	0

#pragma address gDAReg=0xFFE00
volatile uint8_t gDAReg[20];

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
void TaskFanRpm( void );
void SetInverterCurrent( uint16_t uA );
void InitDac(void);
void SetFanPwm(uint16_t val);
void SendResponse( uint8_t *data );
void TaskInput( stInput *in );
void SetDacValue( uint16_t val );
void ControlFan( uint16_t setRpm );
uint16_t ReadAd(void);
uint16_t GetFanRpm( void );
void debug(void);
/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */
	debug();
    while (1U){
		static stInput sInput;
		static uint8_t sFanRpm;
		static uint16_t sInvPow;
		
		TaskFanRpm();
		
		if( gInterval ){
			gInterval --;
			TaskInput( &sInput );
			
			adTmp[cyc] = (uint16_t)(ADCR >> 6U);
			if( cyc == 15 ) cyc = 0;
			else cyc ++;
			
			
			if( gIsReceived ){
				uint16_t *pReg;
				gIsReceived = 0;
				sTimeOut = 1000;
				gRev.cmd |= 0x80;
				switch( gRev.cmd & 0x3F ){
				case 0x0B:		// SoftwareData
					gRev.param0 = 2;		// ModelNo
					gRev.param1 = SOFT_VER;
					gRev.param2 = SOFT_REV;
					SendResponse( (uint8_t *)&gRev );
					break;
				case 0x11:		// StatusData
					// Input Data
					sInput.ivON = (gRev.param0 == 0x5A) ?1 :0 ;
					sInvPow = (uint16_t)gRev.param2<<8 | gRev.param1;
					sInput.fanON = (gRev.param3 == 0x5A) ?1 :0 ;
					sFanRpm = gRev.param4;
					if( sFanRpm < 10 ) sFanRpm = 100;
					else if( sFanRpm > 200 ) sFanRpm = 2000;
					else sFanRpm *= 10;
					SendResponse( (uint8_t *)&gRev );
					break;
				case 0x01:		// ReadData
					pReg = (uint16_t *)&gRev.param0;
					pReg[0] = 0x0000;		// インバータ回転数（今回は０固定）
					pReg[1] = GetFanRpm();	// Fan回転数
					pReg[2] = ReadAd();		// 冷媒圧力センサAD値					
					gRev.param6 = 0;		// インバータステータス
					if( sInput.ivON )  gRev.param6 |= 0x01;
					if( sInput.ivFLB ) gRev.param6 |= 0x02;
					if( sInput.ivOUT ) gRev.param6 |= 0x04;
					SendResponse( (uint8_t *)&gRev );
					break;
				default:
					break;
				}
			}else{
				if( sTimeOut ) sTimeOut --;
				else{
					sInput.ivON = 0;
					sInput.fanON = 0;
				}

			}

			if( sInput.commEMStop ){
				sInput.ivON = 0;
				sInput.fanON = 0;
			}
			
			if( sInput.ivON ){
				oIvF(0);			// 接点開放で動作許可
				SetInverterCurrent(sInvPow);
			}else{
				oIvF(1);			// 接点閉で動作停止
				SetInverterCurrent(0);
			}
			
			if( sInput.fanON ){
				ControlFan( sFanRpm );
			}else{
				ControlFan( 0 );
			}
		}
    }
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    R_IT_Start();				// インターバルタイマ
	R_ADC_Start();				// 冷媒高圧センサ
	R_TAU0_Channel0_Start();	// Fan指令用PWM出力
	R_TAU0_Channel3_Start();	// インバータOUT出力パルス受付
	InitDac();					// インバータ制御用DAC出力関係
	R_TMR_RD0_Start();			// Fan回転数パルス受付
	R_UART0_Start();			// RS485
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

void TaskFanRpm( void ){
	if( gCapEnable ){
		gCapEnable = 0;
		rpmTmp[rpmCyc] = gFanCaptureValue;
		rpmCyc = (rpmCyc+1) &0x03;
	}
}

// 周期をuS単位で取得する。
uint32_t ReadFanRpm( void ){
	uint32_t tmp = 0;
	uint8_t i=4;
	while(i--) tmp += rpmTmp[i];
	return tmp>>2;
}

// 冷媒高圧センサの電圧を取得する（1chだけなので、チャンネル指定なし）
uint16_t ReadAd(void){
	uint16_t sum = 0,i=16;
	while(i--) sum += adTmp[i];
	return sum/16;
}

// インバータに出力する電流値を設定する。
const float DAC2CURRENT	= 5.302865;		// 1DAC当りの電流値換算
void SetInverterCurrent( uint16_t uA ){
	SetDacValue( (uint16_t)((float)uA/DAC2CURRENT) +1 );
}

// DAの設定値を処理する（256*20段階まで設定可能）
void SetDacValue( uint16_t val ){
	uint8_t dac=val/20;
	uint8_t sumTmp=val%20,sum=0,i;
	
	for( i=0; i<20; i++ ){
		sum += sumTmp;
		if( sum >= 20 ){
			gDAReg[i] = dac +1;
			sum -= 0;
		}else{
			gDAReg[i] = dac;
		}
	}
}

// DAC設定関係(DAC/TAU01/DTC)の初期化
void InitDac(void){
	uint8_t i=20;
	while(i--) gDAReg[i] = 0;
	R_TAU0_Channel1_Start();
	R_DTCD0_Start();
	R_DAC0_Start();
}

// 通信のデータを、整形して送信する。
void SendResponse( uint8_t *data ){
	static uint8_t buf[14];
	uint8_t i=0,sum=0;

	P3 |= 0x01;		// RS485 DE/RE
	buf[i++] = 0xFF;
	buf[i++] = 0xFF;
	for(; i<12; i++){
		buf[i] = data[i-2];
		sum += data[i-2];
	}
	buf[i++] = sum;	
	buf[i++] = 0xFE;
	
	R_UART0_Send( buf, sizeof(buf) );
}

// FANに送るPWMをタイマ値で指定する。最大値保護あり
void SetFanPwm(uint16_t val){
	if( val > TDR00 ) val = TDR00+1;
	TDR02 = val;
}

// ECFANの回転数を入手する
uint16_t GetFanRpm( void ){
	// ダミーデータです
	static int16_t dmyNow = 0;
	dmyNow += (int16_t)(TDR02-100)*0.3;
	if( dmyNow <0 ) dmyNow = 0;
	return (uint16_t)dmyNow;
}

// ECFANの回転制御を行う（擬似PI制御）
#define FANPWM_PGAIN	0.3f
#define FANPWM_IGAIN	0.5f
#define FANPWM_SPAN		5
void ControlFan( uint16_t setRpm ){
	static uint16_t Pid_i,exRpm,Cnt;
	int16_t tmp;
	uint16_t rpm = GetFanRpm();
	uint16_t pid_p,pid;
	
	if( Cnt < FANPWM_SPAN ) Cnt ++;
	else{
		Cnt = 0;
		tmp = (setRpm-rpm) -(rpm-exRpm);
		if( tmp < 0 ) Pid_i = 0;
		else Pid_i = (uint16_t)( tmp *FANPWM_IGAIN );
		exRpm = rpm;
	}
	
	if( rpm > setRpm ) pid = 0;
	else{
		pid_p = (setRpm -rpm) *FANPWM_PGAIN;
		pid = pid_p + Pid_i;
	}
	SetFanPwm(pid);
}

// デジタル入力処理
#define CHATA	100
void TaskInput( stInput *in ){
	static uint16_t Cnt[3];
	stInput tmp;
	
	tmp.ivFLB = (P0&0x02) ?0 :1 ;		// P01
	tmp.ivOUT = (P3&0x02) ?0 :1 ;		// P31
	tmp.commEMStop = (P6&0x02) ?1 :0 ;	// P61
	
	if( in->ivFLB == tmp.ivFLB ) Cnt[0] = 0;
	else{
		if( Cnt[0] < CHATA ) Cnt[0] ++;
		else in->ivFLB = tmp.ivFLB;
	}
	
	if( in->ivOUT == tmp.ivOUT ) Cnt[1] = 0;
	else{
		if( Cnt[1] < CHATA ) Cnt[1] ++;
		else in->ivOUT = tmp.ivOUT;
	}
	
	if( in->commEMStop == tmp.commEMStop ) Cnt[2] = 0;
	else{
		if( Cnt[2] < CHATA ) Cnt[2] ++;
		else in->commEMStop = tmp.commEMStop;
	}
}

void debug(void){
	const uint8_t d[] = {0x0B,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	uint8_t sum = 0;
	uint8_t i,wait;
	
	RXD0 = 0xff;
	SRIF0 = 1;
	for(wait=0;wait<10;wait++);
	RXD0 = 0xff;
	SRIF0 = 1;
	for(wait=0;wait<10;wait++);
	for(i=0;i<10;i++){
		sum += d[i];
		RXD0 = d[i];
		SRIF0 = 1;
	for(wait=0;wait<10;wait++);
	}	
	RXD0 = sum;
	SRIF0 = 1;
	for(wait=0;wait<10;wait++);
	RXD0 = 0xFE;
	SRIF0 = 1;
}

/* End user code. Do not edit comment generated here */
