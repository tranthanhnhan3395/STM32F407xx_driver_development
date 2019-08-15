/**
*@file stm32f407xx_uart.c
*@brief provide APIs for UART communications.
*
*This header file provide APIs for communication between stm32f407xx MCU and I/O devices through UART protocol.
*
*@author Tran Thanh Nhan
*@date 15/08/2019
*/

#include "stm32f407xx_uart.h"

extern uint32_t RCC_get_PCLK_value(uint8_t APBx);
extern uint32_t RCC_get_PLL_output (void);

/***********************************************************************
Private function: USARTDIV 's integer part calculator
***********************************************************************/
static uint16_t USARTDIV_integer_part_calc (uint32_t periphCLK, uint32_t baudRate)
{
	uint16_t integerVal = periphCLK/(baudRate*16);
	return integerVal;
}

/***********************************************************************
Private function: USARTDIV 's fractional part calculator
***********************************************************************/
static uint8_t USARTDIV_fractional_part_calc (uint32_t periphCLK, uint32_t baudRate)
{
	double fixedPointVal = (double)periphCLK/(double)(baudRate*16);
	uint16_t integerVal = periphCLK/(baudRate*16);
	uint8_t fractionalVal = (fixedPointVal-integerVal)*16;
	return fractionalVal;
}

/***********************************************************************
UART clock enable/disable
***********************************************************************/
void UART_CLK_ctr(USART_TypeDef *UARTxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE)
	{
		if(UARTxPtr == USART1){
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		}else if(UARTxPtr == USART2){
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		}else if(UARTxPtr == USART3){
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
		}else if(UARTxPtr == UART4){
			RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
		}else if(UARTxPtr == UART5){
			RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
		}else if(UARTxPtr == USART6){
			RCC->APB2ENR |= RCC_APB2ENR_USART6EN;			
		}	
	}else{
		if(UARTxPtr == USART1){
			RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
		}else if(UARTxPtr == USART2){
			RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
		}else if(UARTxPtr == USART3){
			RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN;
		}else if(UARTxPtr == UART4){
			RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
		}else if(UARTxPtr == UART5){
			RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN;
		}else if(UARTxPtr == USART6){
			RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;		
		}	
	}
}

/***********************************************************************
UART peripheral enable/disable
***********************************************************************/
void UART_periph_ctr(USART_TypeDef *UARTxPtr, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		UARTxPtr->CR1 |= USART_CR1_UE;
	}else{
		UARTxPtr->CR1 &= ~(USART_CR1_UE);
	}
}

/***********************************************************************
Set/clear UART interrupt enable bit 
***********************************************************************/
void UART_intrpt_ENbit_ctrl(USART_TypeDef *UARTxPtr,uint8_t setOrClear)
{
}

/***********************************************************************
Initialize UART communication
***********************************************************************/
void UART_init(UART_Handle_t *UARTxHandlePtr)
{
	/*enable clock for UART peripheral*/
	UART_CLK_ctr(UARTxHandlePtr->UARTxPtr,ENABLE);
	
	/*disable UART peripheral for initilization*/
	UART_periph_ctr(UARTxHandlePtr->UARTxPtr,DISABLE);
	
	/*config word length*/
	uint8_t option = UARTxHandlePtr->UARTxConfigPtr->wordLength;
	UARTxHandlePtr->UARTxPtr->CR1 &= ~(USART_CR1_M);
	UARTxHandlePtr->UARTxPtr->CR1 |= option<<USART_CR1_M_Pos;
	
	/*config UART mode*/
	option = UARTxHandlePtr->UARTxConfigPtr->mode;
	UARTxHandlePtr->UARTxPtr->CR1 &= ~USART_CR1_TE;
	UARTxHandlePtr->UARTxPtr->CR1 &= ~USART_CR1_RE;
	if(option == UART_TX){
		UARTxHandlePtr->UARTxPtr->CR1 |= USART_CR1_TE;
	}else if(option == UART_RX){
		UARTxHandlePtr->UARTxPtr->CR1 |= USART_CR1_RE;
	}else if(option == UART_TX_RX){
		UARTxHandlePtr->UARTxPtr->CR1 |= USART_CR1_TE;
		UARTxHandlePtr->UARTxPtr->CR1 |= USART_CR1_RE;
	}

	/*config parity control*/
	option = UARTxHandlePtr->UARTxConfigPtr->parityCtrl;
	UARTxHandlePtr->UARTxPtr->CR1 &= ~(USART_CR1_PCE);
	if(option == UART_ODD_PARCTRL || option == UART_EVEN_PARCTRL){
		UARTxHandlePtr->UARTxPtr->CR1 |= USART_CR1_PCE;
		UARTxHandlePtr->UARTxPtr->CR1 &= ~(USART_CR1_PS);
		UARTxHandlePtr->UARTxPtr->CR1 |= option<<USART_CR1_PS_Pos;
	}
	
	/*config hardware flow control*/
	option = UARTxHandlePtr->UARTxConfigPtr->flowCtrl;
	UARTxHandlePtr->UARTxPtr->CR3 &= ~USART_CR3_CTSE;
	UARTxHandlePtr->UARTxPtr->CR3 &= ~USART_CR3_RTSE;
	if(option == UART_RTS_FLOWCTRL){
		UARTxHandlePtr->UARTxPtr->CR3 |= USART_CR3_RTSE;
	}else if(option == UART_CTS_FLOWCTRL){
		UARTxHandlePtr->UARTxPtr->CR3 |= USART_CR3_CTSE;
	}else if(option == UART_RTS_CTS_FLOWCTRL){
		UARTxHandlePtr->UARTxPtr->CR3 |= USART_CR3_CTSE;
		UARTxHandlePtr->UARTxPtr->CR3 |= USART_CR3_RTSE;
	}
	
	/*config number of stop bit*/
	option = UARTxHandlePtr->UARTxConfigPtr->stopBit;
	UARTxHandlePtr->UARTxPtr->CR2 &= ~(USART_CR2_STOP);
	UARTxHandlePtr->UARTxPtr->CR2 |= option<<USART_CR2_STOP_Pos;
	
	/*config baudrate*/
	uint32_t baudRate = UARTxHandlePtr->UARTxConfigPtr->baudRate;
	uint32_t periphCLK = 0;
	if(UARTxHandlePtr->UARTxPtr == USART1 || UARTxHandlePtr->UARTxPtr == USART6){
		periphCLK = RCC_get_PCLK_value(APB2);
	}else if(UARTxHandlePtr->UARTxPtr == USART2 ||UARTxHandlePtr->UARTxPtr == USART3 || UARTxHandlePtr->UARTxPtr == UART4 || UARTxHandlePtr->UARTxPtr == UART5){
		periphCLK = RCC_get_PCLK_value(APB1);
	}
	UARTxHandlePtr->UARTxPtr->BRR = 0;
	UARTxHandlePtr->UARTxPtr->BRR |= USARTDIV_integer_part_calc(periphCLK,baudRate)<<USART_BRR_DIV_Mantissa_Pos;
	UARTxHandlePtr->UARTxPtr->BRR |= USARTDIV_fractional_part_calc(periphCLK,baudRate)<<USART_BRR_DIV_Fraction_Pos;
	
	/*enable UART peripheral*/
	UART_periph_ctr(UARTxHandlePtr->UARTxPtr,ENABLE);
}

/***********************************************************************
Deinitialize I2C communication
***********************************************************************/
void UART_deinit(USART_TypeDef *UARTxPtr)
{
	if(UARTxPtr == USART1){
		RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
		RCC->AHB2RSTR &= ~(RCC_APB2RSTR_USART1RST);
	}else if(UARTxPtr == USART2){
		RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_USART2RST);
	}else if(UARTxPtr == USART3){
		RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_USART3RST);	
	}else if(UARTxPtr == UART4){
		RCC->APB1RSTR |= RCC_APB1RSTR_UART4RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_UART4RST);	
	}else if(UARTxPtr == UART5){
		RCC->APB1RSTR |= RCC_APB1RSTR_UART5RST;
		RCC->AHB1RSTR &= ~(RCC_APB1RSTR_UART5RST);	
	}else if(UARTxPtr == USART6){
		RCC->APB2RSTR |= RCC_APB2RSTR_USART6RST;
		RCC->AHB2RSTR &= ~(RCC_APB2RSTR_USART6RST);	
	}
}

/***********************************************************************
UART send data 
***********************************************************************/
void UART_send(USART_TypeDef *UARTxHandlePtr, uint8_t *txDataPtr,uint32_t Length);
	
/***********************************************************************
UART receive data
***********************************************************************/
void UART_receive(UART_Handle_t *UARTxHandlePtr, uint8_t *rxDataPtr, uint32_t Length);

/***********************************************************************
UART send data(interrup base)
***********************************************************************/
uint8_t UART_send_intrpt (UART_Handle_t *UARTxHandlePtr, uint8_t *txData,uint32_t Length);

/***********************************************************************
UART receive data(interrup base)
***********************************************************************/
uint8_t UART_receive_intrpt (UART_Handle_t *UARTxHandlePtr, uint8_t *rxData, uint32_t Length);

/***********************************************************************
Enable or disable UART peripheral 's interrupt vector in NVIC
***********************************************************************/
void UART_intrpt_vector_ctrl (uint8_t IRQnumber, uint8_t enOrDis)
{
	if(enOrDis == ENABLE){
		if(IRQnumber <= 31){
			NVIC->ISER[0] |= (1<<IRQnumber);
		}
		else if(IRQnumber > 31 && IRQnumber <= 63){
			NVIC->ISER[1] |= (1<<(IRQnumber%32));
		}
		else if(IRQnumber > 63 && IRQnumber <= 95){
			NVIC->ISER[2] |= (1<<(IRQnumber%64));
		}
	}else{
		if(IRQnumber <= 31){
			NVIC->ICER[0] |= (1<<IRQnumber);
		}
		else if(IRQnumber > 31 && IRQnumber <= 63){
			NVIC->ICER[1] |= (1<<(IRQnumber%32));
		}
		else if(IRQnumber > 63 && IRQnumber <= 95){
			NVIC->ICER[2] |= (1<<(IRQnumber%64));
		}
	}
}

/***********************************************************************
Config priority for UART peripheral 's interrupt 
***********************************************************************/
void UART_intrpt_priority_config(uint8_t IRQnumber, uint8_t priority)
{
	uint8_t registerNo = IRQnumber/4;
	uint8_t section = IRQnumber%4;
	
	NVIC->IP[registerNo] &= ~(0xFF << (8*section));
	NVIC->IP[registerNo] |= (priority << (8*section + NUM_OF_IPR_BIT_IMPLEMENTED));
}

/***********************************************************************
UART interrupt handler 
***********************************************************************/
void UART_intrpt_handler (UART_Handle_t *UARTxHandlePtr)
{
}

/***********************************************************************
inform application of UART event or error
@Note: this is to be define in user application
***********************************************************************/
__attribute__((weak)) void UART_application_event_callback (UART_Handle_t *I2CxHandlePtr,uint8_t event) 
{
}
