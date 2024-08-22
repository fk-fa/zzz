
#ifndef __LORA_H__
#define __LORA_H__

#include "data_type.h"
#include "ald_conf.h"

///////////////////////////////////
#define LORA_CONT_PORT GPIOB
#define LORA_CONT_PIN GPIO_PIN_8

#define SET_LORA_CONT		ald_gpio_write_pin(LORA_CONT_PORT, LORA_CONT_PIN, 1)
#define RESET_LORA_CONT		ald_gpio_write_pin(LORA_CONT_PORT, LORA_CONT_PIN, 0)

#define LORA_TX_PORT GPIOB
#define LORA_TX_PIN GPIO_PIN_6
#define LORA_RX_PORT GPIOB
#define LORA_RX_PIN GPIO_PIN_7

///////////////////////////


///////////////////////////////////
extern uint8_t modbus_address;		//

extern uint8_t u485_okflag;//
extern uint8_t test1;

extern uint8_t ofk_buff[UTMD_DATA_NUM_MAX0];
extern uint8_t ofk_length;
extern uint8_t ofk_comm_bak;


extern uart_handle_t lora_h_uart;
////////////////////////////
#define LORA_SIGN_CR	'\r'	//0x0d
#define LORA_SIGN_LF	'\n'	//0x0a
#define LORA_SIGN_Q		'?'	//

#define LORA_SIGN_NO		0	//
#define LORA_SIGN_MI		1	//
#define LORA_SIGN_MM		2	//
#define LORA_SIGN_MR		3	//
#define LORA_SIGN_SN		4	//
#define LORA_SIGN_BR		5	//
#define LORA_SIGN_BL		6	//
#define LORA_SIGN_STATUS	7	//
#define LORA_SIGN_JOINMODE	8	//
#define LORA_SIGN_DEVEUI	9	//
#define LORA_SIGN_APPEUI	10	//
#define LORA_SIGN_APPKEY	11	//
#define LORA_SIGN_DEVADDR	12	//
#define LORA_SIGN_APPSKEY	13	//
#define LORA_SIGN_NWKSKEY	14	//
#define LORA_SIGN_FRBDMASK	15	//
#define LORA_SIGN_ULDLMODE	16	//
#define LORA_SIGN_WORKMODE	17	//
#define LORA_SIGN_CLASS	18	//
#define LORA_SIGN_JOIN		19	//
#define LORA_SIGN_CONFIRM	20	//
#define LORA_SIGN_APPPORT	21	//
#define LORA_SIGN_DATARATE	22	//
#define LORA_SIGN_RSSI		23	//
#define LORA_SIGN_NBTRIALS	24	//
#define LORA_SIGN_RM		25	//
#define LORA_SIGN_TXP		26	//
#define LORA_SIGN_ADR		27	//
#define LORA_SIGN_RXP		28	//
#define LORA_SIGN_RX1DELAY	29	//
#define LORA_SIGN_NUMMUT	30	//
#define LORA_SIGN_ILOGLVL	31	//
#define LORA_SIGN_KEYSPROT	32	//
#define LORA_SIGN_SEND		33	//
#define LORA_SIGN_RECE		34	//

#define LORA_SET_NO			0	//
#define LORA_SET_ASK		1	//
#define LORA_SET_DATA		2	//

#define LORA_COMM_NO		0	//
#define LORA_COMM_INIT		1	//
#define LORA_COMM_SEND		2	//

#define LORA_COMM_INIT_MAX		9	//
#define LORA_COMM_SEND_MAX		2	//



extern uint8_t lora_para_sign;
extern uint8_t lora_set_sign;
extern uint8_t lora_set_comm;
extern uint8_t lora_set_commnum;

extern uint8_t lora_set_bak1;
extern uint8_t lora_set_bak2;

extern uint16_t lora_sfreq;//发射频率
extern uint8_t lora_sdata_rate;//速率
extern uint8_t lora_sbandwidth;//带宽
extern uint8_t lora_scode_rate;//编码率
extern uint8_t lora_spwr;//发射功率
extern uint8_t lora_siqconverted;//IQ转换功能

extern uint16_t lora_rfreq;//接收频率
extern uint8_t lora_rdata_rate;//速率
extern uint8_t lora_rbandwidth;//带宽
extern uint8_t lora_rcode_rate;//编码率
extern uint8_t lora_riqconverted;//IQ转换功能

extern uint16_t lora_local_addr;//本地地址
extern uint16_t lora_target_addr;//目标地址

extern uint8_t lora_sleep_mode;//睡眠模式

///////////////////////////////////
extern void lora_send_complete(uart_handle_t *arg);
extern void lora_recv_complete(uart_handle_t *arg);
extern void lora_error(uart_handle_t *arg);

///////////////////////////////////
extern void modbus_uartinit(void);
extern void smodbus_rece_info(void);

////////////////////////////
extern void smodbus_send_back_frame(void);

///////////////////////////////////
extern void lora_upload(void);
extern void set_lora_askinfo(const uint8_t *dat);
extern void set_lora_datainfo(const uint8_t *dat);
extern void lora_comm_control(uint8_t utmd_chan);
///////////////////////////////////




#endif

