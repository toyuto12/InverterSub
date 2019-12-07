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
* File Name    : r_cg_timer.c
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
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* For TAU0_ch3 pulse measurement */
extern volatile uint32_t g_tau0_ch3_width;
/* TMRD0 input capture mode */
extern volatile uint32_t g_tmrd0_active_width_a;
extern volatile uint32_t g_tmrd0_inactive_width_a;
extern volatile uint32_t g_tmrd0_active_width_b;
extern volatile uint32_t g_tmrd0_inactive_width_b;
extern volatile uint32_t g_tmrd0_active_width_c;
extern volatile uint32_t g_tmrd0_inactive_width_c;
extern volatile uint32_t g_tmrd0_active_width_d;
extern volatile uint32_t g_tmrd0_inactive_width_d;
extern volatile uint32_t g_tmrd0_active_width_elc;
extern volatile uint32_t g_tmrd0_inactive_width_elc;
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TAU0_Create
* Description  : This function initializes the TAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Create(void)
{
    TAU0EN = 1U;    /* supplies input clock */
    TPS0 = _0000_TAU_CKM0_FCLK_0 | _00E0_TAU_CKM1_FCLK_14 | _0000_TAU_CKM2_FCLK_1 | _0000_TAU_CKM3_FCLK_8;
    /* Stop all channels */
    TT0 = _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON |
          _0008_TAU_CH3_STOP_TRG_ON | _0200_TAU_CH1_H8_STOP_TRG_ON | _0800_TAU_CH3_H8_STOP_TRG_ON;
    /* Mask channel 0 interrupt */
    TMMK00 = 1U;    /* disable INTTM00 interrupt */
    TMIF00 = 0U;    /* clear INTTM00 interrupt flag */
    /* Mask channel 1 interrupt */
    TMMK01 = 1U;    /* disable INTTM01 interrupt */
    TMIF01 = 0U;    /* clear INTTM01 interrupt flag */
    /* Mask channel 1 higher 8 bits interrupt */
    TMMK01H = 1U;    /* disable INTTM01H interrupt */
    TMIF01H = 0U;    /* clear INTTM01H interrupt flag */
    /* Mask channel 2 interrupt */
    TMMK02 = 1U;    /* disable INTTM02 interrupt */
    TMIF02 = 0U;    /* clear INTTM02 interrupt flag */
    /* Mask channel 3 interrupt */
    TMMK03 = 1U;    /* disable INTTM03 interrupt */
    TMIF03 = 0U;    /* clear INTTM03 interrupt flag */
    /* Mask channel 3 higher 8 bits interrupt */
    TMMK03H = 1U;    /* disable INTTM03H interrupt */
    TMIF03H = 0U;    /* clear INTTM03H interrupt flag */
    /* Channel 0 is used as master channel for PWM output function */
    TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_MASTER |
            _0000_TAU_TRIGGER_SOFTWARE | _0001_TAU_MODE_PWM_MASTER;
    TDR00 = _7CFF_TAU_TDR00_VALUE;
    TO0 &= ~_0001_TAU_CH0_OUTPUT_VALUE_1;
    TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;
    /* Channel 2 is used as slave channel for PWM output function */
    TMR02 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE |
            _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
    TDR02 = _0000_TAU_TDR02_VALUE;
    TOM0 |= _0004_TAU_CH2_OUTPUT_COMBIN;
    TOL0 &= ~_0004_TAU_CH2_OUTPUT_LEVEL_L;
    TO0 &= ~_0004_TAU_CH2_OUTPUT_VALUE_1;
    TOE0 |= _0004_TAU_CH2_OUTPUT_ENABLE;
    /* Channel 1 used as interval timer */
    TMR01 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_16BITS_MODE |
            _0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
    TDR01 = _0C7F_TAU_TDR01_VALUE;
    TOM0 &= ~_0002_TAU_CH1_OUTPUT_COMBIN;
    TOL0 &= ~_0002_TAU_CH1_OUTPUT_LEVEL_L;
    TO0 &= ~_0002_TAU_CH1_OUTPUT_VALUE_1;
    TOE0 &= ~_0002_TAU_CH1_OUTPUT_ENABLE;
    /* Channel 3 is used to measure input pulse interval */
    TMR03 = _8000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_16BITS_MODE |
            _0100_TAU_TRIGGER_TIMN_VALID | _0000_TAU_TIMN_EDGE_FALLING | _0004_TAU_MODE_CAPTURE |
            _0000_TAU_START_INT_UNUSED;
    TOM0 &= ~_0008_TAU_CH3_OUTPUT_COMBIN;
    TOL0 &= ~_0008_TAU_CH3_OUTPUT_LEVEL_L;
    TO0 &= ~_0008_TAU_CH3_OUTPUT_VALUE_1;
    TOE0 &= ~_0008_TAU_CH3_OUTPUT_ENABLE;
    NFEN1 &= (uint8_t)~_08_TAU_CH3_NOISE_ON;    /* disable using noise filter of TI03 pin input signal */
    /* Set TO02 pin */
    POM1 &= 0x7FU;
    PMC1 &= 0x7FU;
    P1 &= 0x7FU;
    PM1 &= 0x7FU;
    /* Set TI03 pin */
    PM3 |= 0x02U;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Start
* Description  : This function starts TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Start(void)
{
    TOE0 |= _0004_TAU_CH2_OUTPUT_ENABLE;
    TS0 |= _0001_TAU_CH0_START_TRG_ON | _0004_TAU_CH2_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Stop
* Description  : This function stops TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Stop(void)
{
    TT0 |= _0001_TAU_CH0_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON;
    TOE0 &= ~_0004_TAU_CH2_OUTPUT_ENABLE;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Start
* Description  : This function starts TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Start(void)
{
    TS0 |= _0002_TAU_CH1_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Stop
* Description  : This function stops TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Stop(void)
{
    TT0 |= _0002_TAU_CH1_STOP_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Start
* Description  : This function starts TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Start(void)
{
    TS0 |= _0008_TAU_CH3_START_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Stop
* Description  : This function stops TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Stop(void)
{
    TT0 |= _0008_TAU_CH3_STOP_TRG_ON;
}

/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Get_PulseWidth
* Description  : This function measures TAU0 channel 3 input pulse width.
* Arguments    : width -
*                    the address where to write the input pulse width
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Get_PulseWidth(uint32_t * const width)
{
    /* For channel 3 pulse measurement */
    *width = g_tau0_ch3_width;
}

/***********************************************************************************************************************
* Function Name: R_TMR_RD0_Create
* Description  : This function initializes the TMRD0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TMR_RD0_Create(void)
{
    TRD0EN = 1U;    /* enable input clock supply */ 
    TRDSTR |= _04_TMRD_TRD0_COUNT_CONTINUES;
    TRDSTR &= (uint8_t)~_01_TMRD_TRD0_COUNT_START;  /* disable TMRD0 operation */
    TRDMK0 = 1U;    /* disable TMRD0 interrupt */
    TRDIF0 = 0U;    /* clear TMRD0 interrupt flag */
    /* Set INTTRD0 level 1 priority */
    TRDPR10 = 0U;
    TRDPR00 = 1U;   
    TRDMR |= _00_TMRD_TRDGRC0_GENERAL | _00_TMRD_TRDGRD0_GENERAL;
    TRDCR0 = _04_TMRD_INETNAL_CLOCK_F32 | _C0_TMRD_COUNTER_CLEAR_TRDGRD;
    TRDIER0 |= _00_TMRD_IMID_DISABLE | _10_TMRD_OVIE_ENABLE;
    TRDIORC0 |= _00_TMRD_TRDGRD_CAPTURE_RISING | _40_TMRD_TRDGRD_CAPTURE;
    /* Set TRDIOD0 pin */
    PM1 |= 0x10U;
}

/***********************************************************************************************************************
* Function Name: R_TMR_RD0_Start
* Description  : This function starts TMRD0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TMR_RD0_Start(void)
{
   volatile uint8_t trdsr_dummy;
    
    TRDIF0 = 0U;    /* clear TMRD0 interrupt flag */
    trdsr_dummy = TRDSR0; /* read TRDSR0 before write 0 */
    TRDSR0 = 0x00U; /* clear TRD0 each interrupt request */
    TRDMK0 = 0U;    /* enable TMRD0 interrupt */
    TRDSTR |= _04_TMRD_TRD0_COUNT_CONTINUES;
    TRDSTR |= _01_TMRD_TRD0_COUNT_START;    /* start TMRD0 counter */
}

/***********************************************************************************************************************
* Function Name: R_TMR_RD0_Stop
* Description  : This function stops TMRD0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TMR_RD0_Stop(void)
{
    volatile uint8_t trdsr_dummy;
    
    TRDSTR |= _04_TMRD_TRD0_COUNT_CONTINUES;
    TRDSTR &= (uint8_t)~_01_TMRD_TRD0_COUNT_START;  /* stop TMRD0 counter */
    TRDMK0 = 1U;    /* disable TMRD0 interrupt */
    TRDIF0 = 0U;    /* clear TMRD0 interrupt flag */
    trdsr_dummy = TRDSR0; /* read TRDSR0 before write 0 */
    TRDSR0 = 0x00U; /* clear TRD0 each interrupt request */
}

/***********************************************************************************************************************
* Function Name: R_TMR_RD0_Get_PulseWidth
* Description  : This function calculates TMRD0 pulse width.
* Arguments    : active_width -
*                    high-level width
*                inactive_width -
*                    low-level width
*                channel -
*                    register used as capture mode
* Return Value : status -
*                    MD_OK, MD_ERROR or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_TMR_RD0_Get_PulseWidth(uint32_t * const active_width,
                                   uint32_t * const inactive_width,
                                   timer_channel_t channel)
{
    MD_STATUS status = MD_OK;

    switch (channel)
    {
        case TMCHANNELA:
        
            if ((TRDIORA0 & _04_TMRD_TRDGRA_CAPTURE) == 0U)
            {
                status = MD_ERROR;
            }
            else
            {
                *active_width = g_tmrd0_active_width_a;
                *inactive_width = g_tmrd0_inactive_width_a;
            }

            break;

        case TMCHANNELB:
        
            if ((TRDIORA0 & _40_TMRD_TRDGRB_CAPTURE) == 0U)
            {
                status = MD_ERROR;
            }
            else
            {
                *active_width = g_tmrd0_active_width_b;
                *inactive_width = g_tmrd0_inactive_width_b;
            }

            break;

        case TMCHANNELC:
        
            if ((TRDIORC0 & _04_TMRD_TRDGRC_CAPTURE) == 0U)
            {
                status = MD_ERROR;
            }
            else
            {
                *active_width = g_tmrd0_active_width_c;
                *inactive_width = g_tmrd0_inactive_width_c;
            }

            break;

        case TMCHANNELD:
        
            if ((TRDIORC0 & _40_TMRD_TRDGRD_CAPTURE) == 0U)
            {
                status = MD_ERROR;
            }
            else
            {
                *active_width = g_tmrd0_active_width_d;
                *inactive_width = g_tmrd0_inactive_width_d;
            }

            break;

        case TMCHANNELELC:
        
            if ((TRDIORC0 & _40_TMRD_TRDGRD_CAPTURE) == 0U)
            {
                status = MD_ERROR;
            }
            else
            {
                *active_width = g_tmrd0_active_width_elc;
                *inactive_width = g_tmrd0_inactive_width_elc;
            }

            break;

        default:

            status = MD_ARGERROR;

            break;
    }

    return (status);
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
