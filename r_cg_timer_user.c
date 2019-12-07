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
* File Name    : r_cg_timer_user.c
* Version      : CodeGenerator for RL78/G14 V2.05.03.02 [06 Nov 2018]
* Device(s)    : R5F104BF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for TAU module.
* Creation Date: 2019/12/06
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_tmr_rd0_interrupt(vect=INTTRD0)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* For TAU0_ch3 pulse measurement */
volatile uint32_t g_tau0_ch3_width = 0UL;
/* TMRD0 input capture mode */
volatile uint32_t g_tmrd0_active_width_a = 0UL;
volatile uint32_t g_tmrd0_inactive_width_a = 0UL;
volatile uint32_t g_tmrd0_active_width_b = 0UL;
volatile uint32_t g_tmrd0_inactive_width_b = 0UL;
volatile uint32_t g_tmrd0_active_width_c = 0UL;
volatile uint32_t g_tmrd0_inactive_width_c = 0UL;
volatile uint32_t g_tmrd0_active_width_d = 0UL;
volatile uint32_t g_tmrd0_inactive_width_d = 0UL;
volatile uint32_t g_tmrd0_active_width_elc = 0UL;
volatile uint32_t g_tmrd0_inactive_width_elc = 0UL;
volatile uint8_t  g_tmrd0_ovf_d = 0U;
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_tmr_rd0_interrupt
* Description  : This function is INTTRD0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tmr_rd0_interrupt(void)
{
    uint8_t trdsr0_temp = TRDSR0;
    uint16_t tmrd_pul_d_cur = TRDGRD0;
    uint8_t trdier0_temp = TRDIER0;

    TRDIER0 = 0x00U;

    if ((TRDSR0 & _10_TMRD0_INTOV_GENERATE_FLAG) == _10_TMRD0_INTOV_GENERATE_FLAG)
    {
        TRDSR0 = trdsr0_temp & (uint8_t)~_10_TMRD0_INTOV_GENERATE_FLAG;
        g_tmrd0_ovf_d += 1U;
    }

    if ((TRDSR0 & _08_TMRD0_INTD_GENERATE_FLAG) == _08_TMRD0_INTD_GENERATE_FLAG)
    {
        TRDSR0 = trdsr0_temp & (uint8_t)~_08_TMRD0_INTD_GENERATE_FLAG;

        if (g_tmrd0_ovf_d == 0U)
        {
            g_tmrd0_active_width_d = (uint32_t)tmrd_pul_d_cur;
        }
        else
        {
            g_tmrd0_active_width_d = (uint32_t)(0x10000UL * (uint32_t)g_tmrd0_ovf_d + (uint32_t)tmrd_pul_d_cur);
            g_tmrd0_ovf_d = 0U;
        }

        g_tmrd0_inactive_width_d = 0UL;
    }

    TRDIER0 = trdier0_temp;
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
