
#include "main.h"

////////////////////////////
uint8_t modbus_address=UTMD_INIT_ADD0;

uint8_t u485_okflag=0;
uint8_t test1=0;

uint8_t ofk_buff[UTMD_DATA_NUM_MAX0];
uint8_t ofk_length=0;
uint8_t ofk_comm_bak=0;



//this is test

uart_handle_t lora_h_uart;
////////////////////////////
const uint8_t lora_genpara1[]={"AT+CGMI"};//读取厂家标识
const uint8_t lora_genpara2[]={"AT+CGMM"};//读取取模组标识
const uint8_t lora_genpara3[]={"AT+CGMR"};//读取版本标识
const uint8_t lora_genpara4[]={"AT+CGSN"};//读取产品序列号标识
const uint8_t lora_genpara5[]={"AT+CGBR"};//波特率

const uint8_t lora_rdpara1[]={"AT+CBL"};//设备电量等级
const uint8_t lora_rdpara2[]={"AT+CSTATUS"};//设备当前状态

const uint8_t lora_setpara1[]={"AT+CJOINMODE"};
const uint8_t lora_setpara2[]={"AT+CDEVEUI"};
const uint8_t lora_setpara3[]={"AT+CAPPEUI"};
const uint8_t lora_setpara4[]={"AT+CAPPKEY"};
const uint8_t lora_setpara5[]={"AT+CDEVADDR"};
const uint8_t lora_setpara6[]={"AT+CAPPSKEY"};
const uint8_t lora_setpara7[]={"AT+CNWKSKEY"};
const uint8_t lora_setpara8[]={"AT+CFREQBANDMASK"};
const uint8_t lora_setpara9[]={"AT+CULDLMODE"};
const uint8_t lora_setpara10[]={"AT+CWORKMODE"};
const uint8_t lora_setpara11[]={"AT+CCLASS"};

const uint8_t lora_setpara12[]={"AT+CJOIN"};
const uint8_t lora_setpara13[]={"AT+CCONFIRM"};
const uint8_t lora_setpara14[]={"AT+CAPPPORT"};
const uint8_t lora_setpara15[]={"AT+CDATARATE"};
const uint8_t lora_setpara16[]={"AT+CRSSI FREQBANDIDX"};
const uint8_t lora_setpara17[]={"AT+CNBTRIALS"};
const uint8_t lora_setpara18[]={"AT+CRM"};
const uint8_t lora_setpara19[]={"AT+CTXP"};
const uint8_t lora_setpara20[]={"AT+CADR"};
const uint8_t lora_setpara21[]={"AT+CRXP"};
const uint8_t lora_setpara22[]={"AT+CRX1DELAY"};
const uint8_t lora_setpara23[]={"AT+CNUMMUTICAST"};
const uint8_t lora_setpara24[]={"AT+ILOGLVL"};
const uint8_t lora_setpara25[]={"AT+CKEYSPROTECT"};

const uint8_t lora_sendpara[]={"AT+DTRX=1,2,3,112233"};






const uint8_t lora_sendpara0[]={"AT+CDEVEUI=D896E0FF00000240"};
const uint8_t lora_sendpara1[]={"AT+CAPPEUI=D896E0E000005203"};
const uint8_t lora_sendpara2[]={"AT+CAPPKEY=077EE45C6E4564D96D76AE55AFD3AA89"};
const uint8_t lora_sendpara3[]={"AT+CCLASS=0"};
const uint8_t lora_sendpara4[]={"AT+CFREQBANDMASK=0001"};

const uint8_t lora_sendpara5[]={"AT+CJOIN=1,1,8,8"};
const uint8_t lora_sendpara6[]={"AT+DTRX=1,2,3,112233"};

const uint8_t lora_sendpara7[]={"AT+CJOINMODE=0"};



//const uint8_t lora_para1[]={"AT+IMQTTMODE?"};
//const uint8_t lora_para1[]={"AT+CTX="};
//const uint8_t lora_para2[]={"AT+CRX="};
//const uint8_t lora_para3[]={"AT+CRXS="};
//const uint8_t lora_para4[]={"AT+CADDRSET="};
//const uint8_t lora_para5[]={"AT+CTXADDRSET="};
//const uint8_t lora_para6[]={"AT+CSLEEP="};

uint8_t lora_para_sign=LORA_SIGN_NO;
uint8_t lora_set_sign=LORA_SET_NO;
uint8_t lora_set_comm=LORA_COMM_NO;
uint8_t lora_set_commnum=LORA_COMM_NO;

uint8_t lora_set_bak1=LORA_COMM_NO;
uint8_t lora_set_bak2=LORA_COMM_NO;

uint16_t lora_sfreq=0;
uint8_t lora_sdata_rate=0;
uint8_t lora_sbandwidth=0;
uint8_t lora_scode_rate=0;
uint8_t lora_spwr=0;
uint8_t lora_siqconverted=0;

uint16_t lora_rfreq=0;
uint8_t lora_rdata_rate=0;
uint8_t lora_rbandwidth=0;
uint8_t lora_rcode_rate=0;
uint8_t lora_riqconverted=0;

uint16_t lora_local_addr=1;
uint16_t lora_target_addr=1;

uint8_t lora_sleep_mode=0;


////////////////////////////
void lora_send_complete(uart_handle_t *arg)
{
	utmd_reset_rece_state(0);
    return;
}

/**
  * @brief  Receive a message complete.
  * @param  arg: Pointer to uart_handle_t structure.
  * @retval None.
  */
void lora_recv_complete(uart_handle_t *arg)
{
	uint8_t rece_dat;
	
	rece_dat=mcu_rx_buff[0][0];
	utmd_receive_data(0,rece_dat);
    return;
}

/**
  * @brief  Occurs error.
  * @param  arg: Pointer to uart_handle_t structure.
  * @retval None.
  */
void lora_error(uart_handle_t *arg)
{
	utmd_reset_rece_state(0);
	return;
}

////////////////////////////
void modbus_uartinit(void)
{
	gpio_init_t x;
	
	////////////////////////////
	/* Initialize tx pin */
	x.mode = GPIO_MODE_OUTPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_2;
	ald_gpio_init(LORA_TX_PORT, LORA_TX_PIN, &x);
	
	/* Initialize rx pin */
	x.mode = GPIO_MODE_INPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_2;
	ald_gpio_init(LORA_RX_PORT, LORA_RX_PIN, &x);
	
#if 0
		/* Initialize en pin */
	x.mode = GPIO_MODE_OUTPUT;
	x.odos = GPIO_PUSH_PULL;
	x.pupd = GPIO_PUSH_UP;
	x.podrv = GPIO_OUT_DRIVE_1;
	x.nodrv = GPIO_OUT_DRIVE_0_1;
	x.flt  = GPIO_FILTER_DISABLE;
	x.type = GPIO_TYPE_TTL;
	x.func = GPIO_FUNC_1;
	ald_gpio_init(LORA_CONT_PORT, LORA_CONT_PIN, &x);
#endif
	////////////////////////////
	lora_h_uart.init.baud = 9600;
#if 0
	switch(utmd_bps[0])
	{
		case 0://
			lora_h_uart.init.baud = 1200;
		break;
		case 1://
			lora_h_uart.init.baud = 2400;
		break;
		case 2://
			lora_h_uart.init.baud = 4800;
		break;
		case 3://
			lora_h_uart.init.baud = 9600;
		break;
		case 4://
			lora_h_uart.init.baud = 19200;
		break;
		case 5://
			lora_h_uart.init.baud = 57600;
		break;
		case 6://
			lora_h_uart.init.baud = 115200;
		break;
		default:
			lora_h_uart.init.baud = 9600;
		break;
	}
#endif
	////////////////////////////
	lora_h_uart.init.parity = UART_PARITY_NONE;
#if 0
	switch(utmd_check[0])
	{
		case 0://
			lora_h_uart.init.parity = UART_PARITY_NONE;
		break;
		case 1://
			lora_h_uart.init.parity = UART_PARITY_ODD;
		break;
		case 2://
			lora_h_uart.init.parity = UART_PARITY_EVEN;
		break;
		default:
			lora_h_uart.init.parity = UART_PARITY_NONE;
		break;
	}
#endif
	////////////////////////////
	
	/* Initialize usart */
	lora_h_uart.perh             = UART0;
	//lora_h_uart.init.baud        = 9600;
	lora_h_uart.init.word_length = UART_WORD_LENGTH_8B;
	lora_h_uart.init.stop_bits   = UART_STOP_BITS_1;
	//lora_h_uart.init.parity      = UART_PARITY_NONE;
	lora_h_uart.init.mode        = UART_MODE_UART;
	lora_h_uart.init.fctl        = UART_HW_FLOW_CTL_DISABLE;
	//lora_h_uart.init.over_sampling = DISABLE;
	lora_h_uart.tx_cplt_cbk      = lora_send_complete;
	lora_h_uart.rx_cplt_cbk      = lora_recv_complete;
	lora_h_uart.error_cbk        = lora_error;
	ald_uart_init(&lora_h_uart);
	ald_mcu_irq_config(UART0_IRQn, 3, ENABLE);

	////////////////////////////
	utmd_dispse_num[0]=0;//U485_DISP_NUM;
	utmd_dispre_num[0]=0;//U485_DISP_NUM;
	utmd_set_rece_state(0);
}

void smodbus_rece_info(void)
{
	
	smodbus_send_back_frame();
}

#if 1
void smodbus_send_back_frame(void)
{
	uint8_t i;
	
	i=utmd_rx_count[0];
	if(i>UTMD_DATA_NUM_MAX0)
	{
		i=UTMD_DATA_NUM_MAX0;
	}
	utmd_tx_length[0]=i;
	for(i=0;i<utmd_tx_length[0];i++)
	{
		utmd_tx_buff[0][i]=utmd_rx_buff[0][i];
	}
	utmd_clr_rx(0);
	utmd_send_command[0]=UTMD_COMM_BACK_FLAG;
	utmd_keep_command[0]=UTMD_COMM_BACK_FLAG;
	
	//utmd_tx_buff[0][1] +=0x10;
	CRC16_MODBUS(utmd_tx_buff[0],utmd_tx_length[0]);
	utmd_tx_buff[0][utmd_tx_length[0]] = MODBUS_CRC[0];
	utmd_tx_length[0]++;
	utmd_tx_buff[0][utmd_tx_length[0]] = MODBUS_CRC[1];
	utmd_tx_length[0]++;
	
//	utmd_send_frame(0);
	utmd_back_sign[0]=0;
	utmd_mulsend_flag[0]=1;
}
#endif
////////////////////////////
////////////////////////////
void lora_upload(void)
{
	if((utmd_send_flag[0])||(utmd_mulsend_flag[0])||(utmd_comm_sign[0]))
	{
		return;
	}
	utmd_comm_sign[0]=0;
	switch(lora_para_sign)
	{
		case LORA_SIGN_MI://
			utmd_tx_length[0]=sizeof(lora_genpara1);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_genpara1);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_MM://
			utmd_tx_length[0]=sizeof(lora_genpara2);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_genpara2);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_MR://
			utmd_tx_length[0]=sizeof(lora_genpara3);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_genpara3);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_SN://
			utmd_tx_length[0]=sizeof(lora_genpara4);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_genpara4);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_BR://
			utmd_tx_length[0]=sizeof(lora_genpara5);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_genpara5);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		////////////////////////////
		case LORA_SIGN_BL://
			utmd_tx_length[0]=sizeof(lora_rdpara1);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_rdpara1);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_STATUS://
			utmd_tx_length[0]=sizeof(lora_rdpara2);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_rdpara2);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		////////////////////////////
		case LORA_SIGN_JOINMODE://
			utmd_tx_length[0]=sizeof(lora_setpara1);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara1);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara7);
				set_lora_datainfo(lora_sendpara7);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_DEVEUI://
			utmd_tx_length[0]=sizeof(lora_setpara2);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara2);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara0);
				set_lora_datainfo(lora_sendpara0);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_APPEUI://
			utmd_tx_length[0]=sizeof(lora_setpara3);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara3);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara1);
				set_lora_datainfo(lora_sendpara1);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_APPKEY://
			utmd_tx_length[0]=sizeof(lora_setpara4);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara4);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara2);
				set_lora_datainfo(lora_sendpara2);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_DEVADDR://
			utmd_tx_length[0]=sizeof(lora_setpara5);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara5);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_APPSKEY://
			utmd_tx_length[0]=sizeof(lora_setpara6);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara6);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_NWKSKEY://
			utmd_tx_length[0]=sizeof(lora_setpara7);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara7);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_FRBDMASK://
			utmd_tx_length[0]=sizeof(lora_setpara8);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara8);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara4);
				set_lora_datainfo(lora_sendpara4);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_ULDLMODE://
			utmd_tx_length[0]=sizeof(lora_setpara9);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara9);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_WORKMODE://
			utmd_tx_length[0]=sizeof(lora_setpara10);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara10);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_CLASS://
			utmd_tx_length[0]=sizeof(lora_setpara11);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara11);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara3);
				set_lora_datainfo(lora_sendpara3);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_JOIN://
			utmd_tx_length[0]=sizeof(lora_setpara12);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara12);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara5);
				set_lora_datainfo(lora_sendpara5);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_CONFIRM://
			utmd_tx_length[0]=sizeof(lora_setpara13);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara13);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_APPPORT://
			utmd_tx_length[0]=sizeof(lora_setpara14);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara14);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_DATARATE://
			utmd_tx_length[0]=sizeof(lora_setpara15);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara15);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_RSSI://
			utmd_tx_length[0]=sizeof(lora_setpara16);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara16);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_NBTRIALS://
			utmd_tx_length[0]=sizeof(lora_setpara17);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara17);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_RM://
			utmd_tx_length[0]=sizeof(lora_setpara18);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara18);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_TXP://
			utmd_tx_length[0]=sizeof(lora_setpara19);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara19);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_ADR://
			utmd_tx_length[0]=sizeof(lora_setpara20);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara20);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_RXP://
			utmd_tx_length[0]=sizeof(lora_setpara21);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara21);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_RX1DELAY://
			utmd_tx_length[0]=sizeof(lora_setpara22);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara22);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_NUMMUT://
			utmd_tx_length[0]=sizeof(lora_setpara23);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara23);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_ILOGLVL://
			utmd_tx_length[0]=sizeof(lora_setpara24);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara24);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_KEYSPROT://
			utmd_tx_length[0]=sizeof(lora_setpara25);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_setpara25);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_SEND://
			utmd_tx_length[0]=sizeof(lora_sendpara);
			if(lora_set_sign==LORA_SET_ASK)
			{
				set_lora_askinfo(lora_sendpara);
			}
			else if(lora_set_sign==LORA_SET_DATA)
			{
				utmd_tx_length[0]=sizeof(lora_sendpara6);
				set_lora_datainfo(lora_sendpara6);
			}
			lora_para_sign=LORA_SIGN_NO;
			lora_set_sign=LORA_SET_NO;
		break;
		case LORA_SIGN_RECE://
			
		break;
		
		////////////////////////////
		default:
		break;
	}
	
	
}
////////////////////////////
void set_lora_askinfo(const uint8_t *dat)
{
	uint8_t i;
	
	for(i=0;i<utmd_tx_length[0]-1;i++)
	{
		utmd_tx_buff[0][i]=dat[i];
	}
	utmd_tx_buff[0][i]=LORA_SIGN_Q;
	utmd_tx_length[0]++;
	i++;
	utmd_tx_buff[0][i]=LORA_SIGN_CR;
	
	//utmd_send_frame(0);
	utmd_mulsend_flag[0]=1;
}

void set_lora_datainfo(const uint8_t *dat)
{
	uint8_t i;
	
	for(i=0;i<utmd_tx_length[0]-1;i++)
	{
		utmd_tx_buff[0][i]=dat[i];
	}
	utmd_tx_buff[0][i]=LORA_SIGN_CR;
	//utmd_tx_length[0]--;
	
	//utmd_send_frame(0);
	utmd_mulsend_flag[0]=1;
}

////////////////////////////
void lora_comm_control(uint8_t utmd_chan)
{
	if((utmd_rx_start_flag[utmd_chan]==0)&&(utmd_send_flag[utmd_chan]==0)
		&&(utmd_send_command[utmd_chan]==UTMD_FRAME_SEND_OVER)
			&&(utmd_back_sign[utmd_chan]==0)&&(utmd_comm_sign[utmd_chan]==0)
				&&(utmd_comm_cnt[utmd_chan]==0)&&(utmd_rxtx_over[utmd_chan]==0))
					//&&(utmd_link_sign[utmd_chan]&&(utmd_comm_cnt[utmd_chan]==0))
	{
		switch(lora_set_comm)
		{
			case LORA_COMM_NO://
			break;
			case LORA_COMM_SEND://
				if(lora_set_commnum<=LORA_COMM_SEND_MAX)
				{
					lora_set_commnum++;
					switch(lora_set_commnum)
					{
						case 1://
							lora_para_sign=LORA_SIGN_JOINMODE;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 2://
							lora_para_sign=LORA_SIGN_JOINMODE;
							//lora_set_sign=LORA_SET_DATA;
							lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
#if 0
						case 3://
							lora_para_sign=LORA_SIGN_JOIN;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 4://
							lora_para_sign=LORA_SIGN_SEND;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
#endif
						case LORA_COMM_NO://
						//break;
						default:
							lora_set_comm=LORA_COMM_NO;
							lora_set_commnum=LORA_COMM_NO;
						break;
					}
				}
				else
				{
					lora_set_comm=LORA_COMM_NO;
					lora_set_commnum=LORA_COMM_NO;
				}
			break;
			case LORA_COMM_INIT://
				if(lora_set_commnum<=LORA_COMM_INIT_MAX)
				{
					lora_set_commnum++;
					switch(lora_set_commnum)
					{
						case 1://
							lora_para_sign=LORA_SIGN_JOINMODE;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 2://
							lora_para_sign=LORA_SIGN_DEVEUI;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 3://
							lora_para_sign=LORA_SIGN_APPEUI;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 4://
							lora_para_sign=LORA_SIGN_APPKEY;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 5://
							lora_para_sign=LORA_SIGN_CLASS;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 6://
							lora_para_sign=LORA_SIGN_FRBDMASK;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 7://
							lora_para_sign=LORA_SIGN_JOIN;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 8://
							lora_para_sign=LORA_SIGN_STATUS;
							//lora_set_sign=LORA_SET_DATA;
							lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case 9://
							lora_para_sign=LORA_SIGN_SEND;
							lora_set_sign=LORA_SET_DATA;
							//lora_set_sign=LORA_SET_ASK;
							lora_set_bak1=lora_para_sign;
							lora_set_bak2=lora_set_sign;
							utmd_comm_sign[utmd_chan]=1;
						break;
						case LORA_COMM_NO://
						//break;
						default:
							lora_set_comm=LORA_COMM_NO;
							lora_set_commnum=LORA_COMM_NO;
						break;
					}
				}
				else
				{
					lora_set_comm=LORA_COMM_NO;
					lora_set_commnum=LORA_COMM_NO;
				}
			break;
			////////////////////////////
			default:
			break;
		}
	}
}
////////////////////////////
#if 0
void set_lora_info1(void)
{
	uint8_t i;
	
	for(i=0;i<7;i++)
	{
		utmd_tx_buff[0][i]=lora_para1[i];
	}
	utmd_tx_buff[0][7]=(lora_sfreq&0xff00)>>8;
	utmd_tx_buff[0][8]=(lora_sfreq&0x00ff);
	utmd_tx_buff[0][9]=lora_sdata_rate;
	utmd_tx_buff[0][10]=lora_sbandwidth;
	utmd_tx_buff[0][11]=lora_scode_rate;
	utmd_tx_buff[0][12]=lora_spwr;
	utmd_tx_buff[0][13]=lora_siqconverted;
	
	utmd_tx_length[0]=7;
	//utmd_send_frame(0);
	utmd_mulsend_flag[0]=1;
}
#endif
////////////////////////////
////////////////////////////

