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
* File Name    : r_cg_userdefine.h
* Version      : CodeGenerator for RL78/G14 V2.05.04.02 [20 Nov 2019]
* Device(s)    : R5F104BF
* Tool-Chain   : CCRL
* Description  : This file includes user definition.
* Creation Date: 2020/02/03
***********************************************************************************************************************/

#ifndef _USER_DEF_H
#define _USER_DEF_H

/***********************************************************************************************************************
User definitions
***********************************************************************************************************************/

/* Start user code for function. Do not edit comment generated here */
typedef struct{
	uint8_t cmd;
	uint8_t adr;
	uint8_t param0;
	uint8_t param1;
	uint8_t param2;
	uint8_t param3;
	uint8_t param4;
	uint8_t param5;
	uint8_t param6;
	uint8_t param7;
} stReceiveData;

typedef struct{
	uint16_t ivFLB			:1 ;
	uint16_t ivOUT			:1 ;
	uint16_t commEMStop		:1 ;
	uint16_t nc0			:5 ;
	uint16_t ivON			:1 ;
	uint16_t fanON			:1 ;
	uint16_t nc1			:6 ;
} stInput;

#define oYOBI(x)		P6 = (x)?P6|0x01 :P6&~0x01		// P60
#define oIvF(x)			P0 = (x)?P0|0x01 :P0&~0x01		// P00

#define oYOBI_HIGH	(P6 |= 0x01)
#define oYOBI_LOW	(P6 &= ~0x01)
#define iYOBI		(P6&0x01)

#define oIvF_HIGH	(P0 |= 0x01)
#define oIvF_LOW	(P0 &= ~0x01)
#define iIvF		(P0&0x01)

#define iEMSTOP		(P6&0x02)
#define iFLB		(P0&0x02)
#define iOUT		(P3&0x02)

extern stReceiveData gRev;
extern uint8_t gIsReceived;
extern uint8_t gCapEnable;
extern uint8_t gInterval;
extern uint16_t sTimeOut;

// TRD
extern uint32_t gFanCaptureValue;

/* End user code. Do not edit comment generated here */
#endif
