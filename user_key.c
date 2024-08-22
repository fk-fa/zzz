#include "main.h"
#include "xiaomi.h"
#include "ald_gpio.h"


void init_params(void);
KEY_Inf  KeyInf = {0};
volatile unsigned char key_flag=0;
/*
bit 7
bit 6
bit 5
bit 4
bit 3
bit 2
bit 1
bit 0

*/

void update_press_keyflag(unsigned char i) //i: key index
{
	switch(i)
	{
		case 0:key_flag |=0x01;break;
		case 1:key_flag |=0x02;break;
		case 2:key_flag |=0x04;break;
		case 3:key_flag |=0x08;break;
		case 4:key_flag |=0x10;break;
		case 5:key_flag |=0x20;break;
		case 6:key_flag |=0x40;break;
		case 7:key_flag |=0x80;break;
	}
	
}

void update_nopress_keyflag(unsigned char i) //i: key index
{
	switch(i)
	{
		case 0:key_flag &=0xfe;break;
		case 1:key_flag &=0xfd;break;
		case 2:key_flag &=0xfb;break;
		case 3:key_flag &=0xf7;break;
		case 4:key_flag &=0xef;break;
		case 5:key_flag &=0xdf;break;
		case 6:key_flag &=0xbf;break;
		case 7:key_flag &=0x7f;break;
	}
}

void init_key_io(void)
{
	gpio_init_t x;
	x.mode = GPIO_MODE_INPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_1;
	ald_gpio_init(GPIOA, GPIO_PIN_10, &x);
	ald_gpio_init(GPIOB, GPIO_PIN_15, &x);
	ald_gpio_init(GPIOB, GPIO_PIN_14, &x);
	ald_gpio_init(GPIOB, GPIO_PIN_13, &x);
	ald_gpio_init(GPIOB, GPIO_PIN_12, &x);
}


unsigned char f_key_scan(void)//
{
	volatile unsigned char GetKeyValue = 0;
	
	if(ald_gpio_read_pin(GPIOB, GPIO_PIN_12)==0)
		GetKeyValue |= PRESSKEY_1; //SUB
	if(ald_gpio_read_pin(GPIOB, GPIO_PIN_13)==0)
		GetKeyValue |= PRESSKEY_2;// ADD
	if(ald_gpio_read_pin(GPIOB, GPIO_PIN_14)==0)
		GetKeyValue |= PRESSKEY_3;//POWER
	if(ald_gpio_read_pin(GPIOB, GPIO_PIN_15)==0)
		GetKeyValue |= PRESSKEY_4;//F
	if(ald_gpio_read_pin(GPIOA, GPIO_PIN_10)==0)
		GetKeyValue |= PRESSKEY_5;//M
	
	
    return GetKeyValue;
}


void  f_key_makesure(unsigned char GetKeyValue)//
{
   	unsigned char i,temp,key_num = 0;
    static unsigned char key_temp = 0;
    static unsigned char key_age = 0;
	static unsigned char nTm_Release = 0;//

    temp = key_temp;//
    key_temp = GetKeyValue;//
	GetKeyValue &= ~PRESS_COM_KEY_1;//
	GetKeyValue &= ~PRESS_COM_KEY_2;//
	GetKeyValue &= ~PRESS_COM_KEY_3;//
	
	
	for(i = 0;i < 8;i++)  //
	{
		if(GetKeyValue & 0x01)
			key_num ++;
		GetKeyValue >>= 1;
		if(key_num >= 3)
		{
			key_temp = 0;
			break;
		}
	}
	

    if(key_temp == 0) //??????????? ??3? 3?????????  ??????????
    {		
			if(nTm_Release <= 10)//100ms
			{
				nTm_Release++;
			}
			else
			{
				KeyInf.value = 0;
				KeyInf.bKeyFlag_Used = 0;
				KeyInf.ResultShortInf = 0;
				KeyInf.ResultlongInf = 0;
				for(i = 0;i < PRESSKEYNUMBER;i++)
					KeyInf.longtime[i]= 0;
				for(i=0;i<PRESSCOMNUMBER;i++)
					KeyInf.comtime[i]=0;
			}
    }
	else
	{
		nTm_Release = 0;	
	}
        
    if(KeyInf.bKeyFlag_Used) //
    {
    	key_temp = 0;
		key_age = 0;
		KeyInf.ResultShortInf = 0;
		KeyInf.ResultlongInf = 0;
		KeyInf.valid = 0;
		KeyInf.state = 0;
		KeyInf.value = 0;
		//KeyInf.bKeyFlag_Used=0;
		for(i = 0;i < PRESSKEYNUMBER;i++)
			KeyInf.longtime[i]= 0;
		for(i=0;i<PRESSCOMNUMBER;i++)
			KeyInf.comtime[i]=0;
	}
		
    if((key_temp ^ temp) == 0)//
    {
        key_age ++;
        if(key_age >= PRESSSHORTKEY)
        {
            key_age = 0;
            KeyInf.value = key_temp;
            KeyInf.state &= KeyInf.value;
			if((KeyInf.value & PRESS_COM_KEY_1)==PRESS_COM_KEY_1)
			{
				KeyInf.comtime[NUM1]++;
				if(KeyInf.comtime[NUM1]>=PRESSLONGKEY)
				{
					KeyInf.ResultlongInf |=PRESS_COM_KEY_1;
					
				}
			}
			else if((KeyInf.value & PRESS_COM_KEY_2)==PRESS_COM_KEY_2)
			{
				KeyInf.comtime[NUM2]++;
				if(KeyInf.comtime[NUM2]>=PRESSLONGKEY)
				{
					KeyInf.ResultlongInf |=PRESS_COM_KEY_2;
					
				}
			}else if((KeyInf.value & PRESS_COM_KEY_3)==PRESS_COM_KEY_3)
			{
				KeyInf.comtime[NUM3]++;
				if(KeyInf.comtime[NUM3]>=PRESSLONGKEY)
				{
					KeyInf.ResultlongInf |=PRESS_COM_KEY_3;
					
				}
			}
			else if(KeyInf.value & PRESSKEY_3)
			{
				KeyInf.longtime[NUM3]++;
				if(KeyInf.longtime[NUM3] >= PRESSLONGKEY)
				{
					KeyInf.ResultlongInf |= PRESSKEY_3;
				}
			}
			else if(KeyInf.value & PRESSKEY_4)
			{
				KeyInf.longtime[NUM4]++;
				if(KeyInf.longtime[NUM4] >= PRESSLONGKEY)
				{
					KeyInf.ResultlongInf |= PRESSKEY_4;
					KeyInf.longtime[NUM4] = 0;
				}
			}
			else if(KeyInf.value & PRESSKEY_5)
			{
				KeyInf.longtime[NUM5]++;
				if(KeyInf.longtime[NUM5] >= (PRESSLONGKEY))
				{
					KeyInf.ResultlongInf |= PRESSKEY_5;
					KeyInf.longtime[NUM5] = 0;
				}
			}
			else if(KeyInf.value & PRESSKEY_2)
			{
				KeyInf.longtime[NUM2]++;
				if(KeyInf.longtime[NUM2] >= PRESSLONGKEY)//????????
				{
					KeyInf.ResultlongInf |= PRESSKEY_2;
					KeyInf.longtime[NUM2] = 0;
				}
			}
			else if(KeyInf.value & PRESSKEY_1)
			{
				KeyInf.longtime[NUM1]++;
				if(KeyInf.longtime[NUM1] >= (PRESSLONGKEY))
				{
					KeyInf.ResultlongInf |= PRESSKEY_1;
					KeyInf.longtime[NUM1] = 0;
				}
			}
			
     }
   }
    else
    {
        key_age = 0; //???????????  ??????0
    }

	KeyInf.valid = KeyInf.value & (KeyInf.state^0xff);
		
	if(((KeyInf.value & PRESSKEY_4)==0)&&(KeyInf.longtime[NUM4]>=1)\
		&&(KeyInf.longtime[NUM4] <= PRESSLONGKEY)) //???????  ??????? ????????  ?? ?????????  ?????? ????
	{
		KeyInf.ResultShortInf |= PRESSKEY_4;
		KeyInf.longtime[NUM4] = 0;
	}
	if(((KeyInf.value & PRESSKEY_3)==0)&&(KeyInf.longtime[NUM3]>=1)\
		&&(KeyInf.longtime[NUM3] <= PRESSLONGKEY))
	{
		KeyInf.ResultShortInf |= PRESSKEY_3;
		KeyInf.longtime[NUM3] = 0;
	}
	
	if(((KeyInf.value & PRESSKEY_5)==0)&&(KeyInf.longtime[NUM5] >=1)\
			&&(KeyInf.longtime[NUM5] <= PRESSLONGKEY))
	{
		KeyInf.ResultShortInf |= PRESSKEY_5;
		KeyInf.longtime[NUM5] = 0;
	}
	
	if(((KeyInf.value & PRESSKEY_2)==0)&&(KeyInf.longtime[NUM2] >=1)\
		&&(KeyInf.longtime[NUM2] <= PRESSLONGKEY))
	{
		KeyInf.ResultShortInf |= PRESSKEY_2;
		KeyInf.longtime[NUM2] = 0;
	}
	if(((KeyInf.value & PRESSKEY_1)==0)&&(KeyInf.longtime[NUM1] >=1)\
		&&(KeyInf.longtime[NUM1] <= PRESSLONGKEY))
	{
		KeyInf.ResultShortInf |= PRESSKEY_1;
		KeyInf.longtime[NUM1] = 0;
	}
}

void reset_wifi(void)
{
	if(KeyInf.ResultlongInf == PRESS_COM_KEY_2)
	{
		BELL_ON();
		BL_ON;
		if(lock_state)
		{
			return;
		}
		KeyInf.bKeyFlag_Used=1;
		mi_send_state= MI_RESTORE;
		mi_wait_state= MI_RESTORE;
		mi_recv_timeout=30;
	}
}
void key_power_on(void)
{
	if(KeyInf.ResultShortInf & PRESSKEY_3)
	{
		
		KeyInf.bKeyFlag_Used=1;
		if(bl_time==0)//
		{
			BL_LIGHT;
			return;
		}
			BELL_ON();
		if(lock_state)
		{
			return;
		}
		BL_ON;
		if(ad_set_no>0)
		{
			ad_set_no=0;
			setting_delay_time=0;
			return;
		}
	
		dis_all_time=0;
		power_state=1;
		ding_fa_flag=1;
		fan_pwm=500;
		if(power_save_type)
		flash_changed_flag=1;

	}else if(KeyInf.ResultlongInf==PRESSKEY_3)
	{
		
		KeyInf.bKeyFlag_Used=1;
		BELL_ON();
		if(lock_state)
		{
			return;
		}
		dis_all_time=10;
	}
}

void key_power_off(void)
{
	if(KeyInf.ResultShortInf & PRESSKEY_3)
	{
		
		KeyInf.bKeyFlag_Used=1;
		if(bl_time==0)//
		{
			BL_LIGHT;
			return;
		}
		BELL_ON();
		if(lock_state)
		return;
		
		BL_LIGHT;
		if(time_set_no>0||prog_set_no>0)
		{
			time_set_no=0;
			setting_delay_time=0;
			prog_set_no=0;
			return;
		}
		power_state=0;
		if(power_save_type)
		flash_changed_flag=1;
		
	}
}

void key_mode(void)
{
	
	if(KeyInf.ResultShortInf == PRESSKEY_5)
	{
		
		KeyInf.bKeyFlag_Used=1;
		if(bl_time==0)//
		{
			BL_LIGHT;
			return;
		}
		BELL_ON();
		if(lock_state)
		return;
		
		
		
		
		flash_changed_flag=1;
	}else if(KeyInf.ResultlongInf==PRESSKEY_5)
	{
		KeyInf.bKeyFlag_Used=1;
		if(lock_state)
		return;
		BELL_ON();
		BL_ON;
		
		if(sensor_select_no==2)
		{
			query_out_time=30;
		}
		
	}
}

void key_ad_mode(void)
{
	if(KeyInf.ResultShortInf == PRESSKEY_5)
	{
		
		//T16Nx_Enable(T16N1);  
		KeyInf.bKeyFlag_Used=1;
		BELL_ON();
		BL_ON;
		if(lock_state)
		{
			return;
		}
		if(ad_set_no>0)
		{
			ad_set_no++;
			SETTING_ON;
			if(ad_set_no>16)
			{
				ad_set_no=1;
			}
		}
	}
}
void key_ad_set(void)
{
	if(KeyInf.ResultlongInf == PRESS_COM_KEY_2)
	{
		
		KeyInf.bKeyFlag_Used=1;
		
		BL_ON;
			BELL_ON();
		if(lock_state)
		{
			return;
		}
	
		if(ad_set_no)
		{
			ad_set_no=0;
		}else
		{
			ad_set_no=1;
			SETTING_ON;
		}
	}
	
}

void key_fan(void)
{
	
	
	if(KeyInf.ResultShortInf == PRESSKEY_4)
	{
		KeyInf.bKeyFlag_Used=1;
		if(bl_time==0)//
		{
			BL_LIGHT;
			return;
		}
			BELL_ON();
		if(lock_state)
		return;
		BL_ON;
		
		
		
	}else if(KeyInf.ResultlongInf==PRESSKEY_4)
	{
		KeyInf.bKeyFlag_Used=1;
		BELL_ON();
		if(lock_state)
		return;
		
		BL_ON;
	
	}
}

void key_add(void)
{
	if(KeyInf.ResultShortInf == PRESSKEY_2)
	{
		
		//T16Nx_Enable(T16N1);  
		KeyInf.bKeyFlag_Used=1;
		if(bl_time==0)//
		{
			BL_LIGHT;
			return;
		}
			BELL_ON();
		if(lock_state)
		return;
		
		
		if(setting_delay_time==0)
		{
			SETTING_ON;
			return;
		}
		SETTING_ON;
		if(power_state)
		{
			BL_ON;
			if(time_set_no>0)
			{
				switch(time_set_no)
				{
					case 1:
						if(minute<59)
						{
							minute++;
							DS1302SingleWrite(1,((minute/10) << 4) + ((minute%10) & 0x0f));
						}
						break;
					case 2:
						if(hour<23)
						{
							hour++;
							DS1302SingleWrite(2,((hour/10) << 4) + ((hour%10) & 0x0f));
						}
						break;
						case 3:
						if(week<7)
						{
							week++;
							DS1302SingleWrite(5,((week/10) << 4) + ((week%10) & 0x0f));
						}
						break;
				}
			}
			else
			{
				
				BL_LIGHT;
				
			}
			
			
		}else
		{
				if(bl_time==0)//
				{
					BL_LIGHT;
					return;
				}
			
			if(ad_set_no>0)
			{
			
				BL_ON;
				flash_changed_flag=1;
				
				switch(ad_set_no)
				{
					case 1:
						if(makeup_temp<9)
						makeup_temp++;
						break;
					case 2:
						if(makeup_rh<9)
						makeup_rh++;
						break;
					case 3:
						if(fan_start_temp<9)
						fan_start_temp++;
						break;
					case 4:
						if(fan_stop_temp<9)
						fan_stop_temp++;
						break;
					case 5:
						if(dinuan_start_temp<9)
						dinuan_start_temp++;
						break;
					case 6:
						if(dinuan_stop_temp<9)
						dinuan_stop_temp++ ;
						
						break;
					case 7:
						 if(power_save_type)
							 power_save_type=0;
						 else
							 power_save_type=1;
						break;
					case 8:
						if(screen_display)
						screen_display=0;
						else
							screen_display=1;
						mi_propeties_update(S5_P11);
						break;
					case 9:
						 max_set_temp++;
						if(max_set_temp>99)
						max_set_temp=35;
						break;
					case 10:
						 if(min_set_temp<20)
							min_set_temp++;
						break;
					case 11://±£»¤ÎÂ²î
						if(device_addr<255)
						device_addr++;
						break;
						
					case 12:
						  if(fan_controled)
							fan_controled=0;
							else
							fan_controled=1;
						break;
					case 13:
						if(display_level)
							display_level=0;
						else
							display_level=1;
						break;
					case 14:
						if(dis_switch_flag)
							dis_switch_flag=0;
						else
							dis_switch_flag=1;
						break;
					case 15:
					  if(machine_color)
							machine_color=0;
						else
							machine_color=1;
						break;
				
						case 16:
							if(out_makeup_temp<9)
								out_makeup_temp++;
							break;
						default:
						break;
					
				}
			}
		}
		
		
	}
}

void key_sub(void)
{
	if(KeyInf.ResultShortInf == PRESSKEY_1)
	{
		if(bl_time==0)//
		{
			BL_LIGHT;
			return;
		}
	 
		KeyInf.bKeyFlag_Used=1;
			BELL_ON();
		if(lock_state)
		return;
		
	
		if(setting_delay_time==0)
		{
			SETTING_ON;
			return;
		}
		SETTING_ON;
		if(power_state)
		{
			if(time_set_no>0)
			{
				switch(time_set_no)
				{
					case 1:
						if(minute>0)
						{
							minute--;
							DS1302SingleWrite(1,((minute/10) << 4) + ((minute%10) & 0x0f));
						}
						break;
					case 2:
						if(hour>0)
						{
							hour--;
							DS1302SingleWrite(2,((hour/10) << 4) + ((hour%10) & 0x0f));
						}
						break;
					case 3:
						if(week>1)
						{
							week--;
							DS1302SingleWrite(5,((week/10) << 4) + ((week%10) & 0x0f));
						}
						break;
				}
			}
			else
			{
				BL_LIGHT;
			}
		}
		else
		{
			if(bl_time==0)//
				{
					BL_LIGHT;
					return;
				}
				
			if(ad_set_no>0)
			{
				BL_ON;
				flash_changed_flag=1;
				switch(ad_set_no)
				{
					
					case 1:
						if(makeup_temp>-9)
						makeup_temp--;
						break;
					case 2:
						if(makeup_rh>-9)
						makeup_rh--;
						break;
					case 3:
						if(fan_start_temp>1)
							fan_start_temp--;
						break;
					case 4:
						if(fan_stop_temp>0)
							fan_stop_temp--;
						break;
					case 5:
						if(dinuan_start_temp>1)
						dinuan_start_temp--;
						break;
					case 6:
						if(dinuan_stop_temp>0)
						dinuan_stop_temp--;
						break;
					
					case 7:
						 if(power_save_type)
							 power_save_type=0;
						 else
							 power_save_type=1;
						break;
					case 8:
						if(screen_display)
							screen_display=0;
						else
							screen_display=1;
							
						break;
					case 9:
						  if(max_set_temp>35)
								max_set_temp--;
						
						break;
					case 10:
						  if(min_set_temp>5)
							min_set_temp--;
						break;
					case 11://·À¶³ÎÂ²î
						  if(device_addr>1)
							device_addr--;
						break;
					case 12:
						 if(fan_controled)
							fan_controled=0;
							else
							fan_controled=1;
						break;
					case 13:
						if(display_level)
							display_level=0;
						else
							display_level=1;
						break;
					case 14:
						if(dis_switch_flag)
							dis_switch_flag=0;
						else
							dis_switch_flag=1;
						break;
						case 15:
					  if(machine_color)
							machine_color=0;
						else
							machine_color=1;
						break;
						case 16:
							if(out_makeup_temp>-9)
							out_makeup_temp--;	
							break;
						default:
							break;
						
				}
			}
		}
		
	}
}

void key_lock(void)
{
	if(KeyInf.ResultlongInf==PRESS_COM_KEY_1)
	{
		BELL_ON();
		BL_ON;
		KeyInf.bKeyFlag_Used=1;
		if(lock_state)
			lock_state=0;
		else
			lock_state=1;
		mi_propeties_update(S4_P1);
		flash_changed_flag=1;
	}
}


void key_all_dis(void)
{
	if(KeyInf.ResultlongInf==PRESS_COM_KEY_3)
	{
		KeyInf.bKeyFlag_Used=1;
		BELL_ON();
		BL_ON;
		if(lock_state)
		{
			return;
		}
	  dis_all_time=15;
	}
}


void key_restore(void)
{
	if(KeyInf.ResultlongInf==PRESS_COM_KEY_3)
	{
		KeyInf.bKeyFlag_Used=1;
		BELL_ON();
		BL_ON;
		if(lock_state)
		{
			return;
		}
		init_params();
		power_state=1;
		flash_changed_flag=1;
		
	}
}



void key_poweroff_light(void)
{
	if(KeyInf.ResultShortInf == PRESSKEY_4)
	{
		KeyInf.bKeyFlag_Used=1;
		if(bl_time==0)
		{
			BL_LIGHT;
			return;
		}
	}
}
void key_on_pack(void)
{
	key_power_off();
	key_mode();
	reset_wifi();
	key_fan();
	key_add();
	key_sub();
	key_lock();
	key_restore();
}

void key_off_pack(void)
{
	key_power_on();
	key_ad_set() ;
	key_ad_mode();
	key_add();
	key_sub();
	key_lock();
	key_poweroff_light();
	key_all_dis();
	
}
