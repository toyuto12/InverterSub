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
* Copyright (C) 2011, 2019 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G14 V2.05.04.02 [20 Nov 2019]
* Device(s)    : R5F104BF
* Tool-Chain   : CCRL
* Description  : This file implements main function.
* Creation Date: 2020/01/30
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

uint32_t rpmTmp[4] = { 0x1FFFF, 0x1FFFF, 0x1FFFF, 0x1FFFF };
uint8_t rpmCyc;

float sPID_p=1.5f,sPID_i=0.20f;

void R_MAIN_UserInit(void);

#define SOFT_VER	1
#define SOFT_REV	1

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
uint16_t ReadFanRpm( void );
uint16_t GetFanRpm( void );
void debug(void);
void SendUartAsciiFormValue( uint32_t value, int8_t keta, uint8_t kisuu );
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
	
    while (1U){
		static stInput sInput;
		static uint16_t sFanRpm;
		static uint16_t sInvPow;
		
		TaskFanRpm();
		
		if( gInterval ){
			gInterval --;
			TaskInput( &sInput );
			//oYOBI_HIGH;			
			
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
					gRev.param0 = 3;		// ModelNo
					gRev.param1 = SOFT_VER;
					gRev.param2 = SOFT_REV;
					SendResponse( (uint8_t *)&gRev );
					break;
				case 0x11:		// StatusData
					// Input Data
					sInput.ivON = (gRev.param0 == 0x5A) ?1 :0 ;
					sInvPow = (uint16_t)gRev.param2<<8 | gRev.param1;
					sInput.fanON = (gRev.param3 == 0xA5) ?1 :0 ;
					if( gRev.param4 < 10 ) sFanRpm = 100;
					else if( gRev.param4 >= 98 ) sFanRpm = 980;
					else sFanRpm = gRev.param4 *10;
					sFanRpm = (uint16_t)(( (1 /((float)sFanRpm /60)) *1000000) /16);
					SendResponse( (uint8_t *)&gRev );
					break;
				case 0x01:		// ReadData
				{
					uint16_t dat;
					gRev.param0 = 0;
					gRev.param1 = 0;		// インバータ回転数（今回は０固定）

					dat = (uint16_t)((1 /((float)ReadFanRpm() /64 /1000)) *60);	// Fan回転数
					gRev.param2 = dat&0xFF;
					gRev.param3 = (dat>>8)&0xFF;

					dat = ReadAd();		// 冷媒圧力センサAD値					
					gRev.param4 = dat&0xFF;
					gRev.param5 = (dat>>8)&0xFF;
					
					gRev.param6 = 0;		// インバータステータス
					if( sInput.ivON )  gRev.param6 |= 0x01;
					if( sInput.ivFLB ) gRev.param6 |= 0x02;
					if( sInput.ivOUT ) gRev.param6 |= 0x04;
					SendResponse( (uint8_t *)&gRev );
					break;
				}
				case 0x13:		// FAN_PIDStatus
				{
					uint16_t tmp;
					tmp = ((uint16_t)gRev.param1<<8) + gRev.param0;
					if( tmp > 999 ) tmp = 999;
					sPID_p = (float)tmp /100;
					tmp = ((uint16_t)gRev.param3<<8) + gRev.param2;
					if( tmp > 999 ) tmp = 999;
					sPID_i = (float)tmp /100;
					SendResponse( (uint8_t *)&gRev );
					break;
				}
				default:
					break;
				}
			}else{
				if( sTimeOut ) sTimeOut --;
				else{
//					sInput.ivON = 0;
//					sInput.fanON = 0;
				}

			}

			if( sInput.ivFLB ){
				sInput.ivON = 0;
			}
			
			if( sInput.commEMStop ){
//				sInput.ivON = 0;
//				sInput.fanON = 0;
			}
			
			if( sInput.ivON ){
				uint16_t tmp,tmp2,tmp3;
				if( sInvPow > 2000 ) tmp = (sInvPow-2000) *0.02;
				else tmp = 0;
				if( sInvPow > 12000 ) tmp2 = (sInvPow-12000) *0.065;
				else tmp2 = 0;
				if( sInvPow > 20000 ) tmp3 = (sInvPow-20000) *0.8;
				else tmp3 = 0;
				
				oIvF(0);			// 接点開放で動作許可
				SetInverterCurrent(sInvPow -tmp +tmp2 +tmp3);
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
	R_ADC_Set_OperationOn();
	R_ADC_Start();				// 冷媒高圧センサ
	R_TAU0_Channel1_Start();	// Fan回転数パルス受付
//	R_TAU0_Channel3_Start();	// インバータOUT出力パルス受付
	InitDac();					// インバータ制御用DAC出力関係
//	R_TMR_RD0_Start();			// Fan指令用PWM出力 -> 制御中のON/OFFに切り替え
	R_UART0_Start();			// RS485
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

uint8_t value2ascii( uint8_t val ){
	if( val < 10 ) return val +'0';
	else if( val < 16 ) return val -10 +'a' ;
	else return ' ';
}

void SendUartAsciiFormValue( uint32_t value, int8_t keta, uint8_t kisuu ){
	static uint8_t sBuf[11];
	uint8_t pos = 8;

	sBuf[9] = '\r';
	sBuf[10] = '\n';
	
	while( (keta-- > 0) || value ){
		if( !value ) sBuf[pos] = (pos==8) ?'0' :' ' ;
		else {
			sBuf[pos] = value2ascii( value%kisuu );
			value /= kisuu;
		}
		pos --;
	}
	R_UART0_Send( &sBuf[pos+1], 12-pos);
}

void SetFanRpm( uint32_t val){
	rpmTmp[rpmCyc] = val;
	rpmCyc = (rpmCyc+1) &0x03;
}

void TaskFanRpm( void ){
	static uint16_t To = 1000;
	static uint8_t IsOverFlow = 0;
	uint32_t tmp;
	
	if( gInterval ){		
		if( To ) To --;
		else {
			SetFanRpm( 62517 );
			IsOverFlow = 1;
			To = 1000;
		}
	}
	
	if( gCapEnable ){
		gCapEnable = 0;
		R_TAU0_Channel1_Get_PulseWidth( &tmp );
		if( !IsOverFlow ) SetFanRpm( tmp );
		To = 1000;
		IsOverFlow = 0;
	}
}

// 周期をuS単位で取得する。
uint16_t ReadFanRpm( void ){
	uint32_t tmp = 0;
	uint8_t i=4;
	while(i--) tmp += rpmTmp[i];
	return (uint16_t)(tmp>>2);
}

// 冷媒高圧センサの電圧を取得する（1chだけなので、チャンネル指定なし）
uint16_t ReadAd(void){
	uint16_t sum = 0,i=16;
	while(i--) sum += adTmp[i];
	return sum/16;
}

// インバータに出力する電流値を設定する。
//const float DAC2CURRENT	= 0.185;		// 1DAC当りの電流値換算
const float DAC2CURRENT	= 0.19425;		// 1DAC当りの電流値換算
void SetInverterCurrent( uint16_t uA ){
	SetDacValue( (uint16_t)((float)uA*DAC2CURRENT) +1 );
//	SetDacValue( uA );
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
	R_TAU0_Channel0_Start();
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
	static uint8_t exVal=0;
	if( val > TRDGRA0 ) val = TRDGRA0+1;

	if( exVal != val ){
		if( val == 0 ) R_TMR_RD0_Stop();
		else if( exVal == 0 ) R_TMR_RD0_Start();
	}
	exVal = val;
	TRDGRB0 = val;
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
#define FANPWM_PGAIN	3.0f
#define FANPWM_IGAIN	0.5f
#define FANPWM_SPAN		10
void ControlFan( uint16_t setRpm ){
	int32_t rpm = ReadFanRpm();
	int32_t pid_d,pid_p,pid_i,pid;
	static uint16_t wait=0,wait2=0;
	static int32_t Diff[2],Integral;
	float iGain = sPID_i;
	
	
	if( !setRpm ){
		Integral = 0;
		SetFanPwm(1);
		return;
	}
	
	if( wait < (setRpm/20)) wait ++;
	else{
		wait = 0;
		Diff[0] = Diff[1];
		Diff[1] = rpm -setRpm;
		if( Diff[1] < 10000 ){
			Integral += ((Diff[0] +Diff[1]) /2);
			//if( Integral > 1000000 ) Integral = 1000000;
			if( Integral < -100000 ) Integral = -100000;
		}
	}
	
	pid_p = (rpm -setRpm) *sPID_p;

	//pid_d = (Diff[1] -Diff[0]) *2.0f;

	if( setRpm > 83333 ) iGain += (float)(setRpm -83333) /208335.0f;
	pid_i = Integral *iGain;
	
//	if( wait2 < 1000 ) wait2 ++;
//	else{
//		wait2 = 0;
//		P3 |= 0x01;		// RS485 DE/REff
//		SendUartAsciiFormValue(rpm*100/64,6,10);	
//	}

	pid = pid_p +pid_i;
	if( pid > 0xFFFF ) pid = 0xFFFF;
	if( pid < 0 ) pid = 0;
	SetFanPwm((uint16_t)pid +2900);
}

// デジタル入力処理
#define CHATA	100
void TaskInput( stInput *in ){
	static uint16_t Cnt[3];
	stInput tmp;
	
	tmp.ivFLB = (P0&0x02) ?1 :0 ;		// P01
	tmp.ivOUT = (P3&0x02) ?0 :1 ;		// P31
	tmp.commEMStop = (P6&0x02) ?1 :0 ;	// P61

	//debug
//	tmp.ivFLB = (P0&0x02) ?0 :1 ;		// P01
//	tmp.ivOUT = (P3&0x02) ?0 :1 ;		// P31
//	tmp.commEMStop = (P6&0x02) ?0 :1 ;	// P61
	
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
