#pragma once

#include "BCM2835Driver.hpp"
#include "stdio.h"
#include <logger/logger.hpp>
// Display resolution
#define EPD_WIDTH 640
#define EPD_HEIGHT 384

// EPD7IN5B commands
#define PANEL_SETTING 0x00
#define POWER_SETTING 0x01
#define POWER_OFF 0x02
#define POWER_OFF_SEQUENCE_SETTING 0x03
#define POWER_ON 0x04
#define POWER_ON_MEASURE 0x05
#define BOOSTER_SOFT_START 0x06
#define DEEP_SLEEP 0x07
#define DATA_START_TRANSMISSION_1 0x10
#define DATA_STOP 0x11
#define DISPLAY_REFRESH 0x12
#define IMAGE_PROCESS 0x13
#define LUT_FOR_VCOM 0x20
#define LUT_BLUE 0x21
#define LUT_WHITE 0x22
#define LUT_GRAY_1 0x23
#define LUT_GRAY_2 0x24
#define LUT_RED_0 0x25
#define LUT_RED_1 0x26
#define LUT_RED_2 0x27
#define LUT_RED_3 0x28
#define LUT_XON 0x29
#define PLL_CONTROL 0x30
#define TEMPERATURE_SENSOR_COMMAND 0x40
#define TEMPERATURE_CALIBRATION 0x41
#define TEMPERATURE_SENSOR_WRITE 0x42
#define TEMPERATURE_SENSOR_READ 0x43
#define VCOM_AND_DATA_INTERVAL_SETTING 0x50
#define LOW_POWER_DETECTION 0x51
#define TCON_SETTING 0x60
#define TCON_RESOLUTION 0x61
#define SPI_FLASH_CONTROL 0x65
#define REVISION 0x70
#define GET_STATUS 0x71
#define AUTO_MEASUREMENT_VCOM 0x80
#define READ_VCOM_VALUE 0x81
#define VCM_DC_SETTING 0x82

extern const unsigned char lut_vcom0[];
extern const unsigned char lut_ww[];
extern const unsigned char lut_bw[];
extern const unsigned char lut_bb[];
extern const unsigned char lut_wb[];

class Epd7in5bDriver : BCM2835Driver
{
public:
    int width;
    int height;

    Epd7in5bDriver()
    {
        reset_pin = RST_PIN;
        dc_pin = DC_PIN;
        cs_pin = CS_PIN;
        busy_pin = BUSY_PIN;
        width = EPD_WIDTH;
        height = EPD_HEIGHT;
    };

    int Init(void)
    {
        if (IfInit() != 0)
        {
            return -1;
        }
        Reset();
        SendCommand(POWER_SETTING);
        SendData(0x37);
        SendData(0x00);
        SendCommand(PANEL_SETTING);
        SendData(0xCF);
        SendData(0x08);
        SendCommand(BOOSTER_SOFT_START);
        SendData(0xC7);
        SendData(0xCC);
        SendData(0x28);
        SendCommand(PLL_CONTROL);
        SendData(0x3a);
        SendCommand(TEMPERATURE_CALIBRATION);
        SendData(0x00);
        SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
        SendData(0x77);
        SendCommand(TCON_SETTING);
        SendData(0x22);
        SendCommand(TCON_RESOLUTION);
        SendData(0x02); //source 640
        SendData(0x80);
        SendData(0x01); //gate 384
        SendData(0x80);
        SendCommand(VCM_DC_SETTING);
        SendData(0x28);    //decide by LUT file
        SendCommand(0xE5); //FLASH MODE
        SendData(0x03);
        SendCommand(POWER_ON);
        WaitUntilIdle();
        return 0;
    }
    void WaitUntilIdle(void)
    {
        while (DigitalRead(busy_pin) == 0)
        { //0: busy, 1: idle
            DelayMs(100);
        }
    }
    void Reset(void)
    {
        DigitalWrite(reset_pin, LOW); //module reset
        DelayMs(200);
        DigitalWrite(reset_pin, HIGH);
        DelayMs(200);
    }
    void DisplayFrame(const unsigned char *frame_buffer)
    {
        unsigned char temp1, temp2, temp3;
        SendCommand(DATA_START_TRANSMISSION_1);
        for (int i = 0; i < this->width / 2 * this->height; i++)
        {
            temp1 = frame_buffer[i];
            temp2 = temp1 & 0xF0;
            if (temp2 == 0xF0)
                temp3 = 0x00;
            else if (temp2 == 0x00)
                temp3 = 0x03;
            else
                temp3 = 0x04;

            temp3 <<= 4;
            temp1 <<= 4;

            temp2 = temp1 & 0xF0;
            if (temp2 == 0xF0)
                temp3 |= 0x00;
            else if (temp2 == 0x00)
                temp3 |= 0x03;
            else
                temp3 |= 0x04;
            SendData(temp3);
        }
        SendCommand(DISPLAY_REFRESH);
        DelayMs(100);
        WaitUntilIdle();
    }
    void DisplayFrame1(const unsigned char *frame_buffer)
    {
        SendCommand(DATA_START_TRANSMISSION_1);
        for (int i = 0; i < this->width / 2 * this->height; i++)
        {
            SendData(frame_buffer[i]);
        }
        SendCommand(DISPLAY_REFRESH);
        DelayMs(100);
        WaitUntilIdle();
    }

    void SendCommand(unsigned char command)
    {
        DigitalWrite(dc_pin, LOW);
        SpiTransfer(command);
    }
    void SendData(unsigned char data)
    {
        DigitalWrite(dc_pin, HIGH);
        SpiTransfer(data);
    }
    void Sleep(void)
    {
        SendCommand(POWER_OFF);
        WaitUntilIdle();
        SendCommand(DEEP_SLEEP);
        SendData(0xa5);
    }

private:
    unsigned int reset_pin;
    unsigned int dc_pin;
    unsigned int cs_pin;
    unsigned int busy_pin;
};


