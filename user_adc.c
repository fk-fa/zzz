/**
  *********************************************************************************
  *
  * @file    main.c
  * @brief   Main file for DEMO
  *
  * @version V1.0.1
  * @date    28 Apr 2022
  * @author  AE Team
  * @note
  *          Change Logs:
  *          Date            Author          Notes
  *          26 Apr 2021     AE Team         The first version
  *          28 Apr 2022     AE Team         modify the system clock
  *
  * Copyright (C) Shanghai Eastsoft Microelectronics Co. Ltd. All rights reserved.
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the License); you may
  * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *********************************************************************************
  */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

#include "ald_gpio.h"

#define ERROR_VALUE 4000

const uint16_t v_temp[]=
{
3505,3480,3454,3428,3401,3372,3344,3314,3284,3253, 
3221,3189,3156,3122,3088,3053,3017,2981,2944,2907, 
2869,2831,2791,2753,2713,2673,2632,2591,2550,2509, 
2468,2426,2385,2343,2301,2259,2217,2176,2134,2093, 
2051,2010,1969,1929,1888,1849,1809,1770,1731,1693, 
1655,1617,1580,1544,1508,1473,1438,1404,1370,1337,
1305,1273,1241,1211,1181,1151,1123, 1095, 1067, 
1040,1014,988,963,938,914,891,868,845,824,803,782, 
762,742,723,704,687,669,652,635,619,603,588,
573,558, 545, 531, 517, 504, 491 
};

/** @addtogroup Projects_Examples_ALD
  * @{
  */

/** @addtogroup Examples
  * @{
  */


adc_handle_t h_adc;

env_t _env;

int32_t adc_value;

volatile uint16_t last_out_temp=0;
volatile unsigned short  temp_temp=0;


/**
  * @brief  Adc normal convert complete.
  * @param  arg: Pointer to adc_handle_t structure.
  * @retval None.
  */
void normal_complete_cbk(adc_handle_t *arg)
{
    uint32_t value;

    value = ald_adc_normal_get_value(&h_adc);
    _env.n_max  = value > _env.n_max ? value : _env.n_max;
    _env.n_min  = value < _env.n_min ? value : _env.n_min;
    _env.n_sum += value;

    if (++_env.n_cnt == 18)
    {
        _env.n_sum = _env.n_sum - _env.n_max - _env.n_min;
        _env.n_vol = _env.n_sum >> 4;
        _env.n_cbk(_env.n_vol);
    }
    else
    {
        ald_adc_normal_start_by_it(&h_adc);
    }

    return;
}

/**
  * @brief  Adc insert convert complete.
  * @param  arg: Pointer to adc_handle_t structure.
  * @retval None.
  */
static void insert_complete_cbk(adc_handle_t *arg)
{
    uint32_t value;

    value = ald_adc_insert_get_value(&h_adc, ADC_ICH_IDX_1);
    _env.i_max  = value > _env.i_max ? value : _env.i_max;
    _env.i_min  = value < _env.i_min ? value : _env.i_min;
    _env.i_sum += value;

    if (++_env.i_cnt == 18)
    {
        _env.i_sum = _env.i_sum - _env.i_max - _env.i_min;
        _env.i_vol = _env.i_sum >> 4;
        _env.i_cbk(_env.i_vol);
    }
    else
    {
        ald_adc_insert_start_by_it(&h_adc);
    }

    return;
}

/**
  * @brief  Adc level out of window complete.
  * @param  arg: Pointer to adc_handle_t structure.
  * @retval None.
  */
static void wdg_cbk(adc_handle_t *arg)
{
    return;
}

/**
  * @brief  Adc error complete.
  * @param  arg: Pointer to adc_handle_t structure.
  * @retval None.
  */
static void error_cbk(adc_handle_t *arg)
{
    return;
}

/**
  * @brief  Adc over complete.
  * @param  arg: Pointer to adc_handle_t structure.
  * @retval None.
  */
static void ovr_cbk(adc_handle_t *arg)
{
    return;
}

/**
  * @brief  Initializate pin of adc module.
  * @retval None.
  */
void adc_pin_init(void)
{
    gpio_init_t x;

    memset(&x, 0, sizeof(gpio_init_t));
    x.mode  = GPIO_MODE_INPUT;
    x.pupd  = GPIO_PUSH_UP;
    x.podrv = GPIO_OUT_DRIVE_6;
    x.nodrv = GPIO_OUT_DRIVE_6;
    x.flt   = GPIO_FILTER_DISABLE;
    x.type  = GPIO_TYPE_CMOS;
    x.func  = GPIO_FUNC_0;
    ald_gpio_init(GPIOB, GPIO_PIN_11, &x);

    return;
}

/**
  * @brief  Initializate ADC.
  * @retval None.
  */
void user_adc_init(void)
{
    adc_nch_conf_t nm_config;
    adc_ich_conf_t ih_config;

    /* clear structures */
    memset(&h_adc, 0x0, sizeof(adc_handle_t));
    memset(&nm_config, 0x0, sizeof(adc_nch_conf_t));
    memset(&ih_config, 0x0, sizeof(adc_ich_conf_t));

    /* Initialize adc */
    h_adc.perh            = ADC0;
    h_adc.init.align      = ADC_DATAALIGN_RIGHT;
    h_adc.init.scan       = DISABLE;
    h_adc.init.cont       = DISABLE;
    h_adc.init.nch_nr     = ADC_NCH_NR_1;
    h_adc.init.disc       = ADC_ALL_DISABLE;
    h_adc.init.disc_nr    = ADC_DISC_NR_1;
    h_adc.init.data_bit   = ADC_CONV_BIT_12;
    h_adc.init.div        = ADC_CKDIV_128;
    h_adc.init.nche_sel   = ADC_NCHESEL_MODE_ALL;
    h_adc.init.n_ref      = ADC_NEG_REF_VSS;
    h_adc.init.p_ref      = ADC_POS_REF_VDD;
    h_adc.normal_cplt_cbk = normal_complete_cbk;
    h_adc.insert_cplt_cbk = insert_complete_cbk;
    h_adc.wdg_cbk         = wdg_cbk;
    h_adc.error_cbk       = error_cbk;
    h_adc.ovr_cbk         = ovr_cbk;
    ald_adc_init(&h_adc);

    /* Initialize normal convert channel */
    nm_config.ch   = ADC_CHANNEL_12;
    nm_config.idx  = ADC_NCH_IDX_1;
    nm_config.samp = ADC_SAMPLETIME_4;
    ald_adc_normal_channel_config(&h_adc, &nm_config);

    /* Initialize insert convert channel */
		/*
    ih_config.ch     = ADC_CHANNEL_2;
    ih_config.idx    = ADC_ICH_IDX_1;
    ih_config.samp   = ADC_SAMPLETIME_4;
    ih_config.offset = 0;
    ih_config.nr     = ADC_ICH_NR_1;
    ih_config.auto_m = DISABLE;
    ald_adc_insert_channel_config(&h_adc, &ih_config);
*/
    return;
}

/**
  * @brief  Normal complete callback function
  * @retval Average voltage.
  */
void normal_complete(int32_t value)
{
//   uint8_t  start = 0;
//    uint8_t  end   = 99;
//    uint8_t mid =0;
	uint8_t index=0;
	if(value==-1)
			return;
	if(value>ERROR_VALUE)
		outsensor_error_flag=1;
	else
	{
	  adc_value=value;
		outsensor_error_flag=0;
		
		if(adc_value>=3505)
	{
		current_out_temp=0;
		
		last_out_temp=0;
		return;
	}

	 if(adc_value<491)
	  {
		current_out_temp=990;
	
		last_out_temp=990;
		return;
	  }
		/*
    while (start<=end) {
      mid = (start+end)/2;
		
      if(adc_value>v_temp[mid]) 
			{
				if(mid>0)
				{
					end = mid-1;
				}
			}else if(adc_value<v_temp[mid]) 
			{
				start = mid+1;
			}
			else 
			{
			 temp_temp=mid*10;
			 if(last_out_temp !=0)
				 current_out_temp=(2 * temp_temp + 8 * last_out_temp)/10;
			 else
				 current_out_temp=temp_temp;
			 last_out_temp=current_out_temp;
			 
			 if(current_out_temp+out_makeup_temp*10>=0)
			 {
				current_out_temp +=out_makeup_temp*10;
			 }
			 return;
			}
 
    }
       //start>end  start = end +1
     temp_temp=((float)end +(float)(adc_value-v_temp[end])/(v_temp[start]-v_temp[end]))*10;
		if(last_out_temp !=0)
		 current_out_temp=(2 * temp_temp + 8 * last_out_temp)/10;
		 else
		 current_out_temp=temp_temp;
		last_out_temp=current_out_temp;
		
		if(current_out_temp+out_makeup_temp*10>=0)
		 {
		  current_out_temp +=out_makeup_temp*10;
		 }
		 */
		 
		 for(index=0;index<99;index++)
		 {
			  if(adc_value>v_temp[index])
				{
					temp_temp=index*10;
					break;
				}
		 }
		 
		 if(last_out_temp==0)
		 {
			 last_out_temp=temp_temp;
			 current_out_temp=last_out_temp;
		 }else
		 {
			 current_out_temp=(4 * temp_temp + 6 * last_out_temp)/10;
			 last_out_temp=current_out_temp;
		 }
		 
		 if(current_out_temp+out_makeup_temp*10>=0)
		 {
		  current_out_temp +=out_makeup_temp*10;
		 }
		 /*
		 if(current_out_temp%10 <3 )
			{
				current_out_temp = (current_out_temp/10)*10;
			}else if(current_out_temp%10 <=7)
			{
				current_out_temp = (current_out_temp/10)*10 +5;
			}else
			{
				current_out_temp = (current_out_temp/10)*10 +10;
			}
		 */
		
		
	}
    _env.flag = 1;
    return;
}

/**
  * @brief  Insert complete callback function
  * @retval Average voltage.
  */
void insert_complete(int32_t value)
{
	  if(value==-1)
			return;
    adc_value=value;
    _env.flag = 1;
    return;
}

/**
  * @brief  Get the average value of the normal channel.
  * @retval Average voltage.
  */
void get_normal_average_voltage(cplt_cbk fn)
{
    _env.n_cnt = 0;
    _env.n_sum = 0;
    _env.n_min = 0xFFFF;
    _env.n_max = 0;
    _env.n_vol = 0;
    _env.n_cbk = fn;
    ald_adc_normal_start_by_it(&h_adc);
    return;
}

/**
  * @brief  Get the average value of the insert channel.
  * @retval Average voltage.
  */
/*
void get_insert_average_voltage(cplt_cbk fn)
{
    _env.i_cnt = 0;
    _env.i_sum = 0;
    _env.i_min = 0xFFFF;
    _env.i_max = 0;
    _env.i_vol = 0;
    _env.i_cbk = fn;
    ald_adc_insert_start_by_it(&h_adc);

    return;
}
*/

