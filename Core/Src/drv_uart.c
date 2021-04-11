#include "main.h"
#include "drv_uart.h"


/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void SetToReceiveMode(const void *pData, unsigned long DataLength )
{
	
  // Set PA9 (USART1_TX) as input
//	GPIOA->MODER = (GPIOA->MODER&~(3<<(9*2))) | (0<<(9*2));
	// TE=0 Transmitter disabled
  // TCIE=0 Transmission complete interrupt disable
	USART1->CR1 &= ~( USART_CR1_TE | USART_CR1_TCIE);
  //DMA Channel disabled
  DMA1_Channel5->CCR = 0;
	// Set up the RX DMA
	DMA1_Channel5->CPAR = (unsigned long)&USART1->RDR;
	DMA1_Channel5->CMAR = (unsigned long)pData;
	DMA1_Channel5->CNDTR = DataLength;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=0 Read from peripheral
	// CIRC=1 Circular mode enabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	DMA1_Channel5->CCR = DMA_CCR_CIRC | DMA_CCR_EN | DMA_CCR_MINC;
  // Enable RX DMA
	USART1->CR3 |= USART_CR3_DMAR;
	// Enable the receiver timeout interrupt 
	USART1->CR1 = USART_CR1_UE | USART_CR1_IDLEIE;
	
}

/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void ReceiveStart(void)
{

  // Set PA9 (USART1_TX) as input
//	GPIOA->MODER = (GPIOA->MODER&~(3<<(9*2))) | (0<<(9*2));
	// TE=0 Transmitter disabled
  // TCIE=0 Transmission complete interrupt disable
	USART1->CR1 &= ~( USART_CR1_TE | USART_CR1_TCIE);
  //TX DMA Channel disabled
  DMA1_Channel5->CCR = 0;

  // Clear the receiver timeout flag
	WRITE_REG(USART1->ICR, USART_ICR_IDLECF);
  // UE=1 USART enable
	// RE=1 Receiver enabled
  // RTOIE=1 Receiver timeout interrupt enabled
	USART1->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_IDLEIE;

}

/*==================================================================================================
name:
funciton:
input;
output:
==================================================================================================*/
void SetToSendMode(const void *pData,unsigned long DataLength)
{
	uint32_t tickstart = 0x7FFFF;
	huart1.gState = HAL_UART_STATE_BUSY_TX;
	// turn off receive
  // RE=0 Receiver disabled
  // IDLEIE=0 IDLE interrupt disabled
	USART1->CR1 &= ~( USART_CR1_RE | USART_CR1_IDLEIE );
	// Set PB6 (USART1_TX) as alternate function
//	GPIOB->CRL = (GPIOB->CRL & ~(0x0F000000)) | (0x0B000000);  // alternate function pull push
	// DMA Channel disabled
	DMA1_Channel4->CCR = 0;
	// Set up the TX DMA
	DMA1_Channel4->CPAR = (unsigned long)&USART1->TDR;
	DMA1_Channel4->CMAR = (unsigned long)pData;
	DMA1_Channel4->CNDTR = DataLength;
	// EN=1 DMA channel enabled
	// TCIE=0 Transfer complete interrupt disabled
	// HTIE=0 Half transfer interrupt disabled
	// TEIE=0 Transfer error interrupt disabled
	// DIR=1 Read from memory
	// CIRC=0 Circular mode disabled
	// PINC=0 Peripheral increment mode disabled
	// MINC=1 Memory increment mode enabled
	// PSIZE=0 8-bit peripheral size
	// MSIZE=0 8-bit memory size
	// PL=0 Low priority
	// MEM2MEM=0 Memory to memory mode disabled
	DMA1_Channel4->CCR = DMA_CCR_EN | DMA_CCR_DIR | DMA_CCR_MINC;
	// Clear the transmission complete flag
	WRITE_REG(USART1->ICR, USART_ICR_TCCF);
	// Enable TX DMA
	USART1->CR3 |= USART_CR3_DMAT;
	// Enable the transmit complete interrupt and start transmit
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_TCIE;
	tickstart = HAL_GetTick();
	while( ((HAL_GetTick() - tickstart) < 1000) && (huart1.gState != HAL_UART_STATE_READY)){};
}
