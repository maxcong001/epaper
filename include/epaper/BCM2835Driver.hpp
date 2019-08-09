#pragma once
#include <bcm2835.h>
// Pin definition
#define RST_PIN 17
#define DC_PIN 25
#define CS_PIN 8
#define BUSY_PIN 24

class BCM2835Driver
{
public:
    static int IfInit(void)
    {
        if (!bcm2835_init())
        {
            return -1;
        }
        bcm2835_gpio_fsel(RST_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(DC_PIN, BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_fsel(BUSY_PIN, BCM2835_GPIO_FSEL_INPT);

        bcm2835_spi_begin();                                        //Start spi interface, set spi pin for the reuse function
        bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);    //High first transmission
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                 //spi mode 0
        bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128); //Frequency
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                    //set CE0
        bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);    //enable cs0
        return 0;
    }

    static void DigitalWrite(int pin, int value)
    {
        bcm2835_gpio_write(pin, value);
    }
    static int DigitalRead(int pin)
    {
        return bcm2835_gpio_lev(pin);
    }
    static void DelayMs(unsigned int delaytime)
    {
        bcm2835_delay(delaytime);
    }
    static void SpiTransfer(unsigned char data)
    {
        bcm2835_spi_transfer(data);
    }
};
