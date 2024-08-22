#include <string.h>


#include "main.h"
#include "ald_gpio.h"
#include "ald_usart.h"

#define POLY 0x8005
unsigned char crc_array[2]={0};
volatile unsigned char temp_addr=0;
volatile unsigned char check_one=0;
volatile unsigned char check_two=0;
volatile unsigned char check_code=0;
volatile uint16_t temp_crc=0;
volatile uint16_t temp;
uart_handle_t h_uart;
volatile uint8_t rx_485_buf[18]={0};
volatile uint8_t tx_485_buf[40]={0};
volatile uint8_t response_flag;//0 

usart_handle_t      h_usart;
usart_clock_init_t  h_clock;

void crc16(unsigned char *ptr,unsigned char len)
{
uint16_t wcrc=0xffff;
unsigned char temp;
unsigned char i=0,j=0;

for(i=0;i<len;i++)
{
temp =*ptr&0xff;
ptr++;
wcrc ^=temp;
  for(j=0;j<8;j++)
  {
     if(wcrc&0x0001)
     {
      wcrc>>=1;
      wcrc ^=0xa001;
     }else
    {
    wcrc >>=1;
    }
  }

}
temp=wcrc;
crc_array[0]=(unsigned char )wcrc;//??
crc_array[1]=(unsigned char )(wcrc>>8);//??

}


#if 0
void wifi_uart_pin_init(void)
{
		gpio_init_t x;

	/* Initialize tx pin */
	x.mode = GPIO_MODE_OUTPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_2;
	ald_gpio_init(GPIOB, GPIO_PIN_6, &x);//42PIN

	/* Initialize rx pin */
	x.mode = GPIO_MODE_INPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_2;
	ald_gpio_init(GPIOB, GPIO_PIN_7, &x);//43 PIN
	
	
		/* Initialize en pin */
	x.mode = GPIO_MODE_OUTPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_1;
	ald_gpio_init(GPIOB, GPIO_PIN_8, &x);

	return;
}
#endif

void modbus_uart_pin_init(void)
{
		gpio_init_t x;

	  /* Initialize tx pin */
    x.mode = GPIO_MODE_OUTPUT;
    x.odos = GPIO_PUSH_PULL;
    x.pupd = GPIO_PUSH_UP;
    x.podrv = GPIO_OUT_DRIVE_1;
    x.nodrv = GPIO_OUT_DRIVE_0_1;
    x.flt  = GPIO_FILTER_DISABLE;
    x.type = GPIO_TYPE_TTL;
    x.func = GPIO_FUNC_3;
    ald_gpio_init(GPIOA, GPIO_PIN_2, &x);

    /* Initialize rx pin */
    x.mode = GPIO_MODE_INPUT;
    x.odos = GPIO_PUSH_PULL;
    x.pupd = GPIO_PUSH_UP;
    x.podrv = GPIO_OUT_DRIVE_1;
    x.nodrv = GPIO_OUT_DRIVE_0_1;
    x.flt  = GPIO_FILTER_DISABLE;
    x.type = GPIO_TYPE_TTL;
    x.func = GPIO_FUNC_3;
    ald_gpio_init(GPIOA, GPIO_PIN_3, &x);
	
	
		/* Initialize en pin */
	x.mode = GPIO_MODE_OUTPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_1;
	ald_gpio_init(GPIOC, GPIO_PIN_0, &x);

	return;
}


void usart_send_complete(usart_handle_t *arg)
{
		//modbus_receive_on();
	  //tx_485_time=10;

    return;
}

/**
  * @brief  Receive a message complete.
  * @param  arg: Pointer to uart_handle_t structure.
  * @retval None.
  */
void usart_recv_complete(usart_handle_t *arg)
{
	//rx_485_length=arg->rx_size;
	//rx_485_time=0;
	
    return;
}

/**
  * @brief  Occurs error.
  * @param  arg: Pointer to uart_handle_t structure.
  * @retval None.
  */
void usart_error(usart_handle_t *arg)
{
		CLEAR_BIT(h_usart.state, USART_STATE_TX_MASK);
		CLEAR_BIT(h_usart.state, USART_STATE_RX_MASK);
	  ald_usart_reset(arg);
	modbus_uart_paras_init();
    return;
}


void modbus_uart_paras_init(void)
{
	//ald_uart_reset(&h_uart);


	 /* Initialize tx_buf */
 

    /* Initialize uart */
    h_usart.perh             = USART0;
    h_usart.init.baud        = 9600;
    h_usart.init.word_length = USART_WORD_LENGTH_8B;
    h_usart.init.stop_bits   = USART_STOP_BITS_1;
    h_usart.init.parity      = USART_PARITY_NONE;
    h_usart.init.mode        = USART_MODE_TX_RX;
    h_usart.init.fctl        = USART_HW_FLOW_CTL_NONE;
    h_usart.tx_cplt_cbk      = usart_send_complete;
    h_usart.rx_cplt_cbk      = usart_recv_complete;
    h_usart.error_cbk        = usart_error;
    ald_usart_init(&h_usart);

    /* Initialize clock */
    h_clock.clk      = USART_CLOCK_DISABLE;
    h_clock.polarity     = USART_CPOL_LOW;
    h_clock.phase        = USART_CPHA_1EDGE;
    h_clock.last_bit     = USART_LAST_BIT_ENABLE;
    ald_usart_clock_init(&h_usart, &h_clock);

    ald_mcu_irq_config(USART0_IRQn, 0, ENABLE);
		

		 
		SET_BIT(h_usart.state, USART_STATE_TX_MASK);
		SET_BIT(h_usart.state, USART_STATE_RX_MASK);
		
		ald_usart_interrupt_config(&h_usart, USART_IT_RXNE, ENABLE);
		ald_usart_interrupt_config(&h_usart, USART_IT_PE, ENABLE);
    ald_usart_interrupt_config(&h_usart, USART_IT_ERR, ENABLE);
    /* Send a message */
    //ald_usart_send_by_it(&h_usart, tx_buf, 8);
    //ald_delay_ms(1000);

    /* Receive a message */
    //ald_usart_recv_by_it(&h_usart, rx_485_buf, 18);
		
		
	/*
	ald_uart_send(&h_uart, tx_buf, 8, 1000);
	ald_delay_ms(1000);

	ald_uart_recv_frame_by_it(&h_uart, rx_buf, 8, 0xFF);

	*/
}


void modbus_send_on(void)
{
	ald_gpio_write_pin(GPIOC, GPIO_PIN_0, 1);
}

void modbus_receive_on(void)
{
	//SET_BIT(h_usart.state, USART_STATE_RX_MASK);
	//ald_usart_interrupt_config(&h_usart, USART_IT_RXNE, ENABLE);
	ald_gpio_write_pin(GPIOC, GPIO_PIN_0, 0);
}

#if 0
void wifi_enable(void){
	gpio_init_t x;
	x.mode = GPIO_MODE_INPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_1;
	ald_gpio_init(GPIOB, GPIO_PIN_8, &x);
}

void wifi_disable(void)
{
	gpio_init_t x;
	x.mode = GPIO_MODE_OUTPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_1;
	ald_gpio_init(GPIOB, GPIO_PIN_8, &x);
	ald_gpio_write_pin(GPIOB, GPIO_PIN_8, 0);
}

void uart_error(uart_handle_t *arg)
{
//	ald_uart_reset(arg);
	uart_paras_init();
	return;
}

void uart_send_complete(uart_handle_t *arg)
{
	
	return;
}

/**
  * @brief  Receive a message complete.
  * @param  arg: Pointer to uart_handle_t structure.
  * @retval None.
  */
void uart_recv_complete(uart_handle_t *arg)
{
	return;
}
void uart_paras_init(void)
{
	//ald_uart_reset(&h_uart);
	memset(&h_uart, 0x0, sizeof(h_uart));
	h_uart.perh             = UART0;
	h_uart.init.baud        = 9600;
	h_uart.init.word_length = UART_WORD_LENGTH_8B;
	h_uart.init.stop_bits   = UART_STOP_BITS_1;
	h_uart.init.parity      = UART_PARITY_NONE;
	h_uart.init.mode        = UART_MODE_UART;
	h_uart.init.fctl        = UART_HW_FLOW_CTL_DISABLE;
	h_uart.tx_cplt_cbk      = uart_send_complete;
	h_uart.rx_cplt_cbk      = uart_recv_complete;
	h_uart.error_cbk        = uart_error;
	
	//h_uart.rx_buf=wifi_uart_rx_buf;
	//h_uart.tx_buf=wifi_uart_tx_buf;
	WRITE_REG(h_uart.perh->IDR, 0xFFF);
	WRITE_REG(h_uart.perh->ICR, 0xFFF);
	SET_BIT(h_uart.perh->FCON, UART_FCON_RFRST_MSK);
  SET_BIT(h_uart.perh->ICR, UART_ICR_RFTH_MSK);
	SET_BIT(h_uart.perh->FCON, UART_FCON_TFRST_MSK);
  SET_BIT(h_uart.perh->ICR, UART_ICR_TFTH_MSK);
	
	SET_BIT(h_uart.perh->ICR, UART_ICR_RFFULL_MSK);
	 
	ald_uart_init(&h_uart);
	ald_mcu_irq_config(UART0_IRQn, 1, ENABLE);
  ald_uart_interrupt_config(&h_uart, UART_IT_RFTH, ENABLE);
	
	/*
	ald_uart_send(&h_uart, tx_buf, 8, 1000);
	ald_delay_ms(1000);

	ald_uart_recv_frame_by_it(&h_uart, rx_buf, 8, 0xFF);

	*/
}
#endif

#if 0
void send_wifi_data(unsigned char n)
{
	 g_tx_count=0;
	 g_tx_length=n;
	ald_uart_clear_flag_status(&h_uart, UART_IF_TFTH);
	
  ald_uart_interrupt_config(&h_uart, UART_IT_TFTH, ENABLE);
}


void uart_test(void)
{
	uint8_t tx_buf[8];
	memset(tx_buf, 0x55, sizeof(tx_buf));
	//ald_uart_send(&h_uart, tx_buf, 8, 1000);
}

#endif


void deal_485data(void)
{
	unsigned char i;
	
	unsigned char start_addr=0;
	unsigned char reg_num=0;
	
	if(rx_485_time==0&&rx_485_length>2)//???? ????
	{
	 
	crc16((uint8_t *)rx_485_buf,rx_485_length-2);
	
	 if(rx_485_buf[0]==device_addr&&crc_array[0]==rx_485_buf[rx_485_length-2]&&crc_array[1]==rx_485_buf[rx_485_length-1])//?????? ???
	 {
		  if(rx_485_buf[1]==0x04)
		  {
			  
			  start_addr=rx_485_buf[3];
		      reg_num=rx_485_buf[5];
			  if((start_addr+reg_num)>6)
			  {
				response_flag=0x0;
			  }else
			  {
				  response_flag=0x04;
			  }
		  }
		  else if(rx_485_buf[1]==0x03)
		  {
		
			start_addr=rx_485_buf[3];
		    reg_num=rx_485_buf[5];
			if((start_addr+reg_num)>16)
			  {
				response_flag=0x0;
			  }else
			  {
				  response_flag=0x03;
			  }
		  }
		  else if(rx_485_buf[1]==0x06)
		  {
			
			start_addr=rx_485_buf[3];
			if(start_addr>15)
			{
				response_flag=0x0;
			}else
			{
				response_flag=0x06;
				flash_changed_flag=1;
			}
			switch(start_addr)
			{
				case 0:
				work_mode=rx_485_buf[5];
				break;
				case 1:
				fan_set_no=rx_485_buf[5];
				break;
				case 2:
				set_temp=(rx_485_buf[4]<<8)+rx_485_buf[5];
				break;
				case 3:
				power_state=rx_485_buf[5];
				if(power_state)
					ding_fa_flag=1;
				break;
				case 4:
				dinuan_work_cmd=rx_485_buf[5];
				break;
				case 5:
				fan_work_cmd=rx_485_buf[5];
				break;
				case 6:
				dinuan_on_seconds=(rx_485_buf[4]<<8)+rx_485_buf[5];
				break;
				case 7:
				dinuan_off_seconds=(rx_485_buf[4]<<8)+rx_485_buf[5];
				break;
				case 8:
				break;
				case 9:
				dinuan_work_mode=rx_485_buf[5];
				break;
				case 10:
				mode_ctrl=rx_485_buf[5];
				break;
				case 11:
				fan_ctrl=rx_485_buf[5];
				break;
				case 12:
				dinuan_ctrl=rx_485_buf[5];
				break;
				case 13:
				dev_type=rx_485_buf[5];
				break;
				case 14:
					ding_fa_flag=rx_485_buf[5];
					break;
				
				case 15:
					fan_pwm=(rx_485_buf[4]<<8)+rx_485_buf[5];
					break;
			}
			
		  }
		
	 }else
	 {
	 response_flag=0;
	 }
	 for(i=0;i<18;i++)
	 {
		 rx_485_buf[i]=0;
	 }
	 tx_485_cnt=0;
	 rx_485_length=0;
	}

	if(response_flag==3)
	{
	  response_flag=0;
		modbus_send_on();//????
	 
	  tx_485_length=0;
	  tx_485_buf[tx_485_length++]=device_addr;
	  tx_485_buf[tx_485_length++]=0x03;
	  tx_485_buf[tx_485_length++]=reg_num*2;
	  for(i=start_addr;i<start_addr+reg_num;i++)
	  {
	  tx_485_buf[tx_485_length++]=set_reg[i][0];
	  tx_485_buf[tx_485_length++]=set_reg[i][1];
	  }

	  crc16((uint8_t *)tx_485_buf,tx_485_length);

	  tx_485_buf[tx_485_length++]=(unsigned char)crc_array[0];
	  tx_485_buf[tx_485_length++]=crc_array[1];


	  tx_485_cnt=0;
	 ald_usart_interrupt_config(&h_usart, USART_IT_TXE, ENABLE);
	 //ald_usart_send_by_it(&h_usart, tx_485_buf, tx_485_length);
	  
	}else if(4==response_flag)
	{
		response_flag=0;
		modbus_send_on();//????
	 
	  tx_485_length=0;
	  tx_485_buf[tx_485_length++]=device_addr;
	  tx_485_buf[tx_485_length++]=0x04;
	  tx_485_buf[tx_485_length++]=reg_num*2;

	  for(i=start_addr;i<start_addr+reg_num;i++)
	  {
	  tx_485_buf[tx_485_length++]=state_reg[i][0];
	  tx_485_buf[tx_485_length++]=state_reg[i][1];
	  }
	  crc16((uint8_t *)tx_485_buf,tx_485_length);

	  tx_485_buf[tx_485_length++]=crc_array[0];
	  tx_485_buf[tx_485_length++]=crc_array[1];


	  tx_485_cnt=0;
	 
	 ald_usart_interrupt_config(&h_usart, USART_IT_TXE, ENABLE);
	}else if(response_flag==0x06)
	{
	  response_flag=0;
		modbus_send_on();//????
	 
	  tx_485_length=0;
	  tx_485_buf[tx_485_length++]=device_addr;
	  tx_485_buf[tx_485_length++]=0x06;
	  tx_485_buf[tx_485_length++]=0x00;
	  tx_485_buf[tx_485_length++]=start_addr;

	  switch(start_addr)
	  {
		  case 0:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=work_mode;
		  break;
		  case 1:
		   tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=fan_set_no;
		  break;
		  case 2:
		
		   tx_485_buf[tx_485_length++]=set_temp>>8;
		  tx_485_buf[tx_485_length++]=set_temp;
		  break;
		  case 3:
		   tx_485_buf[tx_485_length++]=0x00;
		   tx_485_buf[tx_485_length++]=power_state;
		  break;
		  case 4:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=dinuan_work_cmd;
		  break;
		  case 5:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=fan_work_cmd;
		  break;
		  case 6:
		  tx_485_buf[tx_485_length++]=dinuan_on_seconds>>8;
		  tx_485_buf[tx_485_length++]=dinuan_on_seconds;
		  break;
		  case 7:
		  tx_485_buf[tx_485_length++]=dinuan_off_seconds>>8;
		  tx_485_buf[tx_485_length++]=dinuan_off_seconds;
		  break;
		  case 8:
		  tx_485_buf[tx_485_length++]=dinuan_work_seconds>>8;
		  tx_485_buf[tx_485_length++]=dinuan_work_seconds;
		  break;
		  case 9:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=dinuan_work_mode;
		  break;
		  case 10:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=mode_ctrl;
		  break;
		  case 11:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=fan_ctrl;
		  break;
		  case 12:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=dinuan_ctrl;
		  break;
		  case 13:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=dev_type;
		  break;
			case 14:
		  tx_485_buf[tx_485_length++]=0x00;
		  tx_485_buf[tx_485_length++]=ding_fa_flag;
		  break;
			case 15:
			tx_485_buf[tx_485_length++]=fan_pwm>>8;
		  tx_485_buf[tx_485_length++]=fan_pwm;
				break;
		  
	  }

	  crc16((uint8_t *)tx_485_buf,tx_485_length);

	  tx_485_buf[tx_485_length++]=crc_array[0];
	  tx_485_buf[tx_485_length++]=crc_array[1];
	   tx_485_cnt=0;
	 
	 ald_usart_interrupt_config(&h_usart, USART_IT_TXE, ENABLE);

	}
	
	
		
}


