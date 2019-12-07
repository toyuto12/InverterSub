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
* File Name    : r_cg_dac.c
* Version      : CodeGenerator for RL78/G14 V2.05.03.02 [06 Nov 2018]
* Device(s)    : R5F104BF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for DAC module.
* Creation Date: 2019/12/06
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_dac.h"
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
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_DAC_Create
* Description  : This function initializes the DA converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DAC_Create(void)
{
    DACEN = 1U;  /* supply DA clock */
    DAM = _00_DA0_CONVERSION_MODE_NORMAL;
    /* Initialize DA0 configuration */
    DACS0 = _00_DA0_CONVERSION_VALUE;
    /* The reset status of ADPC is analog input, so it's unnecessary to set. */
    /* Set ANO0 pin */
    PM2 |= 0x04U;
}

/***********************************************************************************************************************
* Function Name: R_DAC0_Start
* Description  : This function enables the DA converter channel 0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DAC0_Start(void)
{
    DACE0 = 1U;  /* enable DA0 conversion */
}

/***********************************************************************************************************************
* Function Name: R_DAC0_Stop
* Description  : This function stops the DA converter channel 0.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_DAC0_Stop(void)
{
    DACE0 = 0U;  /* stop DA0 conversion */
}

/***********************************************************************************************************************
* Function Name: R_DAC0_Set_ConversionValue
* Description  : This function sets the DA converter channel 0 value.
* Arguments    : reg_value -
*                    value of conversion
* Return Value : None
***********************************************************************************************************************/
void R_DAC0_Set_ConversionValue(uint8_t reg_value)
{
    DACS0 = reg_value;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
