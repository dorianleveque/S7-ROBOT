#include "exceptions.h"
#include "sam3n.h"
#include "system_sam3n.h"

#include "asf.h"
#include "conf_board.h"
#include "conf_clock.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Exception Table */
__attribute__ ((section(".vectors")))
IntFunc exception_table[] = {

	/* Configure Initial Stack Pointer, using linker-generated symbols */
	(IntFunc) (&_estack),
	Reset_Handler,

	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0, 0, 0, 0,         /* Reserved */
	SVC_Handler,
	DebugMon_Handler,
	0,                  /* Reserved  */
	PendSV_Handler,
	SysTick_Handler,

	/* Configurable interrupts  */
	SUPC_Handler,    /* 0  Supply Controller */
	RSTC_Handler,    /* 1  Reset Controller */
	RTC_Handler,     /* 2  Real Time Clock */
	RTT_Handler,     /* 3  Real Time Timer */
	WDT_Handler,     /* 4  Watchdog Timer */
 	PMC_Handler,     /* 5  PMC */
	EFC_Handler,     /* 6  EEFC */
	Dummy_Handler,   /* 7  Reserved */
 	UART0_Handler,   /* 8  UART0 */
	UART1_Handler,   /* 9  UART1 */
	Dummy_Handler,   /* 10 Reserved */
	PIOA_Handler,    /* 11 Parallel IO Controller A */
	PIOB_Handler,    /* 12 Parallel IO Controller B */
#ifdef ID_PIOC
	PIOC_Handler,    /* 13 Parallel IO Controller C */
#else
	Dummy_Handler,
#endif
	USART0_Handler,  /* 14 USART 0 */
#ifdef ID_USART1
	USART1_Handler,  /* 15 USART 1 */
#else
	Dummy_Handler,
#endif
	Dummy_Handler,   /* 16 Reserved */
	Dummy_Handler,   /* 17 Reserved */
	Dummy_Handler,   /* 18 Reserved */
	TWI0_Handler,    /* 19 TWI 0 */
	TWI1_Handler,    /* 20 TWI 1 */
 	SPI_Handler,     /* 21 SPI */
	Dummy_Handler,   /* 22 Reserved */
	TC0_Handler,     /* 23 Timer Counter 0 */
	TC1_Handler,     /* 24 Timer Counter 1 */
	TC2_Handler,     /* 25 Timer Counter 2 */
#ifdef ID_TC3
	TC3_Handler,     /* 26 Timer Counter 3 */
#else
	Dummy_Handler,
#endif
#ifdef ID_TC4
 	TC4_Handler,     /* 27 Timer Counter 4 */
#else
	Dummy_Handler,
#endif
#ifdef ID_TC5
	TC5_Handler,     /* 28 Timer Counter 5 */
#else
	Dummy_Handler,
#endif
	ADC_Handler,     /* 29 ADC controller */
#ifdef ID_DACC
	DACC_Handler,    /* 30 DAC controller */
#else
	Dummy_Handler,
#endif
	PWM_Handler,	 /* 31 PWM */
	Dummy_Handler    /* 32 not used */
};

/* TEMPORARY PATCH FOR SCB */
#define SCB_VTOR_TBLBASE_Pos               29                            /*!< SCB VTOR: TBLBASE Position */
#define SCB_VTOR_TBLBASE_Msk               (1UL << SCB_VTOR_TBLBASE_Pos) /*!< SCB VTOR: TBLBASE Mask */

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
	uint32_t *pSrc, *pDest;

	/* Initialize the relocate segment */
	pSrc = &_etext;
	pDest = &_srelocate;

	if (pSrc != pDest) {
		for (; pDest < &_erelocate;) {
			*pDest++ = *pSrc++;
		}
	}

	/* Clear the zero segment */
	for (pDest = &_szero; pDest < &_ezero;) {
		*pDest++ = 0;
	}

	/* Set the vector table base address */
	pSrc = (uint32_t *) & _sfixed;
	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

	if (((uint32_t) pSrc >= IRAM_ADDR) && ((uint32_t) pSrc < IRAM_ADDR + IRAM_SIZE)) {
		SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos;
	}

	/* Initialize the C library */
	__libc_init_array();
	


	sysclk_init();
	board_init();
	

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1);
}
