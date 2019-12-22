#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "HAL/hal_spi.h"
#include "HWO/max31865.h"

void _chip_select_init(void)
{
    // Pin 2.1
    PM5CTL0 &= ~LOCKLPM5;

    P2DIR |= 0x02;
    P2OUT |= 0x02;

    PM5CTL0 |= LOCKLPM5;
}

void _chip_select(bool select)
{
    if(select){
        P2OUT &= ~0x02;
    } else {
        P2OUT |= 0x02;
    }
}

void _charge_time_delay()
{
    volatile uint16_t i = 0;
    for(i = 0; i<0x1FFF;i++);
}

void _conversion_time_delay()
{
    volatile uint16_t i = 0;
    for(i = 0; i<0x1FFF;i++);

    // TODO: check if plausible: wait until DRDY goes low (active -> conversion completed)
}

int main(void)
{
    volatile uint16_t temp = 0;
    max31865_t Temperature;

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    _chip_select_init();

    hal_spi_init(spi_mode_MASTER,
                 spi_clk_source_ACLK,
                 spi_clk_mode_1,
                 0,
                 true);

    // init Temperature object

    // void hal_spi_trx(const uint8_t* txblock, uint8_t* rxblock, uint8_t len)

    max31865_init(&Temperature,
                  _chip_select,
                  hal_spi_trx_byte,
                  _charge_time_delay,
                  _conversion_time_delay,
                  1000,
                  430,  // Rref on breakout board
                  0,
                  0xFFFF,
                  false,    // 3 wire mode
                  false);

    temp = max31865_readADC(&Temperature);


	while(1)
	{
	    temp = max31865_readADC(&Temperature);
	    //_charge_time_delay();
	}
	
	return 0;
}
