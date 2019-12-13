#pragma once
#include <string>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cpprest/http_msg.h>
#include <logger/logger.hpp>
#include "epaper/epd7in5bpaint.hpp"

#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t
#define WHITE 0xFF
#define BLACK 0x00
#define RED BLACK

/*
                #define EPDPAINT_BLACK 0x0
                #define EPDPAINT_RED 0x4
                #define EPDPAINT_WHITE 0x3
                */
/*Bitmap file header   14bit*/
typedef struct BMP_FILE_HEADER
{
    UWORD bType;                         //File identifier
    UDOUBLE bSize;                       //The size of the file
    UWORD bReserved1;                    //Reserved value, must be set to 0
    UWORD bReserved2;                    //Reserved value, must be set to 0
    UDOUBLE bOffset;                     //The offset from the beginning of the file header to the beginning of the image data bit
} __attribute__((packed)) BMPFILEHEADER; // 14bit

/*Bitmap information header  40bit*/
typedef struct BMP_INFO
{
    UDOUBLE biInfoSize;      //The size of the header
    UDOUBLE biWidth;         //The width of the image
    UDOUBLE biHeight;        //The height of the image
    UWORD biPlanes;          //The number of planes in the image
    UWORD biBitCount;        //The number of bits per pixel
    UDOUBLE biCompression;   //Compression type
    UDOUBLE bimpImageSize;   //The size of the image, in bytes
    UDOUBLE biXPelsPerMeter; //Horizontal resolution
    UDOUBLE biYPelsPerMeter; //Vertical resolution
    UDOUBLE biClrUsed;       //The number of colors used
    UDOUBLE biClrImportant;  //The number of important colors
} __attribute__((packed)) BMPINFOHEADER;

/*Color table: palette */
typedef struct RGB_QUAD
{
    UBYTE rgbBlue;     //Blue intensity
    UBYTE rgbGreen;    //Green strength
    UBYTE rgbRed;      //Red intensity
    UBYTE rgbReversed; //Reserved value
} __attribute__((packed)) BMPRGBQUAD;
enum class epaperRet
{
    SUCCESS = 0,
    SUCCESS_CREATED,
    NOT_SUPPORT,
    BAD_REQUEST

};
class epaperProcessor
{
public:
    static UBYTE GUI_ReadBmp(const char *path, UWORD Xstart, UWORD Ystart)
    {
        FILE *fp;                    //Define a file pointer
        BMPFILEHEADER bmpFileHeader; //Define a bmp file header structure
        BMPINFOHEADER bmpInfoHeader; //Define a bmp info header structure

        // Binary file open
        if ((fp = fopen(path, "rb")) == NULL)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "Cann't open the file!");
            }

            exit(0);
        }

        // Set the file pointer from the beginning
        fseek(fp, 0, SEEK_SET);
        fread(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, fp); //sizeof(BMPFILEHEADER) must be 14
        fread(&bmpInfoHeader, sizeof(BMPINFOHEADER), 1, fp); //sizeof(BMPFILEHEADER) must be 50
        printf("pixel = %d * %d\r\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);

        UWORD Image_Width_Byte = (bmpInfoHeader.biWidth % 8 == 0) ? (bmpInfoHeader.biWidth / 8) : (bmpInfoHeader.biWidth / 8 + 1);
        UWORD Bmp_Width_Byte = (Image_Width_Byte % 4 == 0) ? Image_Width_Byte : ((Image_Width_Byte / 4 + 1) * 4);
        UBYTE Image[Image_Width_Byte * bmpInfoHeader.biHeight];
        memset(Image, 0xFF, Image_Width_Byte * bmpInfoHeader.biHeight);

        // Determine if it is a monochrome bitmap
        unsigned int readbyte = bmpInfoHeader.biBitCount;
        if (readbyte != 1)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "the bmp Image is not a monochrome bitmap!");
            }

            exit(0);
        }

        // Determine black and white based on the palette
        UWORD i;
        UWORD Bcolor, Wcolor;
        UWORD bmprgbquadsize = pow(2, bmpInfoHeader.biBitCount); // 2^1 = 2
        BMPRGBQUAD bmprgbquad[bmprgbquadsize];                   //palette
        // BMPRGBQUAD bmprgbquad[2];        //palette

        for (i = 0; i < bmprgbquadsize; i++)
        {
            // for(i = 0; i < 2; i++) {
            fread(&bmprgbquad[i * 4], sizeof(BMPRGBQUAD), 1, fp);
        }
        if (bmprgbquad[0].rgbBlue == 0xff && bmprgbquad[0].rgbGreen == 0xff && bmprgbquad[0].rgbRed == 0xff)
        {
            Bcolor = BLACK;
            Wcolor = WHITE;
        }
        else
        {
            Bcolor = WHITE;
            Wcolor = BLACK;
        }

        // Read image data into the cache
        UWORD x, y;
        UBYTE Rdata;
        fseek(fp, bmpFileHeader.bOffset, SEEK_SET);
        for (y = 0; y < bmpInfoHeader.biHeight; y++)
        { //Total display column
            for (x = 0; x < Bmp_Width_Byte; x++)
            { //Show a line in the line
                if (fread((char *)&Rdata, 1, readbyte, fp) != readbyte)
                {
                    perror("get bmpdata:\r\n");
                    break;
                }
                if (x < Image_Width_Byte)
                { //bmp
                    Image[x + (bmpInfoHeader.biHeight - y - 1) * Image_Width_Byte] = Rdata;
                    // printf("rdata = %d\r\n", Rdata);
                }
            }
        }
        fclose(fp);

        // Refresh the image to the display buffer based on the displayed orientation
        UBYTE color, temp;
        for (y = 0; y < bmpInfoHeader.biHeight; y++)
        {
            for (x = 0; x < bmpInfoHeader.biWidth; x++)
            {
                if (x > Paint::getInstance()->GetWidth() || y > Paint::getInstance()->GetHeight())
                {
                    break;
                }
                temp = Image[(x / 8) + (y * Image_Width_Byte)];
                color = (((temp << (x % 8)) & 0x80) == 0x80) ? Bcolor : Wcolor;

                Paint::getInstance()->DrawPixel(Xstart + x, Ystart + y, color);
            }
        }
        return 0;
    }
    /*************************************************************************

*************************************************************************/
    static UBYTE GUI_ReadBmp_4Gray(const char *path, UWORD Xstart, UWORD Ystart)
    {
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the function GUI_ReadBmp_4Gray");
        }
        FILE *fp;                    //Define a file pointer
        BMPFILEHEADER bmpFileHeader; //Define a bmp file header structure
        BMPINFOHEADER bmpInfoHeader; //Define a bmp info header structure

        // Binary file open
        if ((fp = fopen(path, "rb")) == NULL)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "Cann't open the file!");
            }
            return 0;
            //exit(0);
        }
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "open file success");
        }

        // Set the file pointer from the beginning
        fseek(fp, 0, SEEK_SET);
        fread(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, fp); //sizeof(BMPFILEHEADER) must be 14
        fread(&bmpInfoHeader, sizeof(BMPINFOHEADER), 1, fp); //sizeof(BMPFILEHEADER) must be 50
        printf("pixel = %d * %d\r\n", bmpInfoHeader.biWidth, bmpInfoHeader.biHeight);

        UWORD Image_Width_Byte = (bmpInfoHeader.biWidth % 4 == 0) ? (bmpInfoHeader.biWidth / 4) : (bmpInfoHeader.biWidth / 4 + 1);
        UWORD Bmp_Width_Byte = (bmpInfoHeader.biWidth % 2 == 0) ? (bmpInfoHeader.biWidth / 2) : (bmpInfoHeader.biWidth / 2 + 1);
        UBYTE Image[Image_Width_Byte * bmpInfoHeader.biHeight * 2];
        memset(Image, 0xFF, Image_Width_Byte * bmpInfoHeader.biHeight * 2);

        // Determine if it is a monochrome bitmap
        int readbyte = bmpInfoHeader.biBitCount;
        printf("biBitCount = %d\r\n", readbyte);
        if (readbyte != 4)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "Bmp image is not a 4-color bitmap!");
            }
            return 0;
            //exit(0);
        }
        // Read image data into the cache
        UWORD x, y;
        UBYTE Rdata;
        fseek(fp, bmpFileHeader.bOffset, SEEK_SET);

        for (y = 0; y < bmpInfoHeader.biHeight; y++)
        { //Total display column
            for (x = 0; x < Bmp_Width_Byte; x++)
            { //Show a line in the line
                if (fread((char *)&Rdata, 1, 1, fp) != 1)
                {
                    perror("get bmpdata:\r\n");
                    break;
                }
                if (x < Image_Width_Byte * 2)
                { //bmp
                    Image[x + (bmpInfoHeader.biHeight - y - 1) * Image_Width_Byte * 2] = Rdata;
                }
            }
        }
        fclose(fp);

        // Refresh the image to the display buffer based on the displayed orientation
        UBYTE color, temp;
        printf("bmpInfoHeader.biWidth = %d\r\n", bmpInfoHeader.biWidth);
        printf("bmpInfoHeader.biHeight = %d\r\n", bmpInfoHeader.biHeight);
        for (y = 0; y < bmpInfoHeader.biHeight; y++)
        {
            for (x = 0; x < bmpInfoHeader.biWidth; x++)
            {
                if (x > Paint::getInstance()->GetWidth() || y > Paint::getInstance()->GetHeight())
                {
                    break;
                }
                temp = Image[x / 2 + y * bmpInfoHeader.biWidth / 2] >> ((x % 2) ? 0 : 4); //0xf 0x8 0x7 0x0
                color = temp >> 2;
                Paint::getInstance()->DrawPixel(Xstart + x, Ystart + y, color); //11  10  01  00
            }
        }
        return 0;
    }

    static epaperRet processImage(web::json::value jValue)
    {
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the image case");
        }
        // {"imagepath":"/xxx/xxx.bmp","positionx":[0,0]}
        auto imagePath = jValue.at("imagepath").as_string();
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the image case, path is : " << imagePath);
        }
        json::array posx = jValue.at("position").as_array();
        int positionx = posx.at(0).as_integer();
        int positiony = posx.at(1).as_integer();
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the image case,  posx: " << positionx << ", posy is : " << positiony);
        }
        GUI_ReadBmp(imagePath.c_str(), positionx, positiony);
        return epaperRet::SUCCESS;
    }
    static epaperRet processLine(web::json::value jValue)
    {
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the line case");
        }
        //{"colour":0,"positionx":[0,0],"positiony":[100,100]}
        int colour = jValue.at("colour").as_integer();
        /*
                #define EPDPAINT_BLACK 0x0
                #define EPDPAINT_RED 0x4
                #define EPDPAINT_WHITE 0x3
                */

        if (colour != 0 && colour != 4 && colour != 3)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupport colour, use white as default, colour is : " << colour);
            }
            colour = EPDPAINT_WHITE;
        }
        json::array posx = jValue.at("positionx").as_array();
        json::array posy = jValue.at("positiony").as_array();
        if (posx.size() != 2 || posy.size() != 2)
        {
            return epaperRet::BAD_REQUEST;
        }
        int posxx = posx.at(0).as_integer();
        int posxy = posx.at(1).as_integer();
        int posyx = posy.at(0).as_integer();
        int posyy = posy.at(1).as_integer();

        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "the line info is : " << posxx << ":" << posxy << ":" << posyx << ":" << posyy << ", colour is : " << colour);
        }
        Paint::getInstance()->DrawLine(posxx, posxy, posyx, posyy, colour);
        return epaperRet::SUCCESS;
    }
    static epaperRet processRotate(int rotate)
    {
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "process rotate : " << rotate);
        }
        Paint::getInstance()->SetRotate(rotate);
        return epaperRet::SUCCESS;
    }
    static epaperRet processCircle(web::json::value jValue)
    {
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the circle case");
        }
        //{"colour":0,"position":[0,0],"radius":100}

        int colour = jValue.at("colour").as_integer();

        bool fill = false;
        if (jValue.has_field("fill"))
        {
            fill = jValue.at("fill").as_bool();
        }
        if (colour != 0 && colour != 4 && colour != 3)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupport colour, use white as default, colour is : " << colour);
            }
            colour = EPDPAINT_WHITE;
        }
        int radius = jValue.at("radius").as_integer();
        json::array pos = jValue.at("position").as_array();

        if (pos.size() != 2)
        {
            return epaperRet::BAD_REQUEST;
        }
        int posx = pos.at(0).as_integer();
        int posy = pos.at(1).as_integer();

        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "the circle info is : " << posx << ":" << posy << ", colour is : " << colour << ", radius is : " << radius);
        }
        if (fill)
        {
            Paint::getInstance()->DrawFilledCircle(posx, posy, radius, colour);
        }
        else
        {
            Paint::getInstance()->DrawCircle(posx, posy, radius, colour);
        }
        return epaperRet::SUCCESS;
    }
    static epaperRet processRectangle(web::json::value jValue)
    {
        //{"colour":0,"position":[0,0],"height":100,"wide":100}
        // note : position is the top left point of rectangle
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the rectangle case");
        }

        int colour = jValue.at("colour").as_integer();

        bool fill = false;
        if (jValue.has_field("fill"))
        {
            fill = jValue.at("fill").as_bool();
        }
        if (colour != 0 && colour != 4 && colour != 3)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupport colour, use white as default, colour is : " << colour);
            }
            colour = EPDPAINT_WHITE;
        }
        int height = jValue.at("height").as_integer();
        int wide = jValue.at("wide").as_integer();
        json::array pos = jValue.at("position").as_array();

        if (pos.size() != 2)
        {
            return epaperRet::BAD_REQUEST;
        }
        int posx = pos.at(0).as_integer();
        int posy = pos.at(1).as_integer();

        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "the rectangle info is : " << posx << ":" << posy << ", colour is : " << colour << ", height is : " << height << ", wide is : " << wide);
        }
        if (fill)
        {
            Paint::getInstance()->DrawFilledRectangle(posx, posy, posx + wide, posy + height, colour);
        }
        else
        {
            Paint::getInstance()->DrawRectangle(posx, posy, posx + wide, posy + height, colour);
        }
        return epaperRet::SUCCESS;
    }
    static epaperRet processPoint(web::json::value jValue)
    {
        // {"colour":0,"position":[0,0]}
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the point case");
        }

        int colour = jValue.at("colour").as_integer();
        if (colour != 0 && colour != 4 && colour != 3)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupport colour, use white as default, colour is : " << colour);
            }
            colour = EPDPAINT_WHITE;
        }
        json::array pos = jValue.at("position").as_array();

        if (pos.size() != 2)
        {
            return epaperRet::BAD_REQUEST;
        }
        int posx = pos.at(0).as_integer();
        int posy = pos.at(1).as_integer();

        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "the point info is : " << posx << ":" << posy << ", colour is : " << colour);
        }
        Paint::getInstance()->DrawPixel(posx, posy, colour);
        return epaperRet::SUCCESS;
    }

    static epaperRet processString(web::json::value jValue)
    {
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "in the string case");
        }
        //{"colour":0,"position":[0,0],"content":"test string", "font":8}

        int colour = jValue.at("colour").as_integer();

        if (colour != 0 && colour != 4 && colour != 3)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupport colour, use white as default, colour is : " << colour);
            }
            colour = EPDPAINT_WHITE;
        }
        int bcolour = EPDPAINT_WHITE;
        if (jValue.has_field("bcolour"))
        {
            bcolour = jValue.at("bcolour").as_integer();
            if (bcolour != 0 && bcolour != 4 && bcolour != 3)
            {
                if (CHECK_LOG_LEVEL(debug))
                {
                    __LOG(debug, "unsupport bcolour, use white as default, bcolour is : " << bcolour);
                }
                bcolour = EPDPAINT_WHITE;
            }
        }

        json::array pos = jValue.at("position").as_array();

        if (pos.size() != 2)
        {
            return epaperRet::BAD_REQUEST;
        }
        int posx = pos.at(0).as_integer();
        int posy = pos.at(1).as_integer();

        auto content = jValue.at("content").as_string();

        int font = jValue.at("font").as_integer();
        Paint::getInstance()->printString(content, font, posx, posy, colour, bcolour);
        return epaperRet::SUCCESS;
    }

    static epaperRet processGroup(web::json::value jValueMsg)
    {
        if (!jValueMsg.is_array())
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "not a group");
            }
            return epaperRet::BAD_REQUEST;
        }

        json::array jValueArray = jValueMsg.as_array();
        for (auto jValueIter = jValueArray.begin(); jValueIter != jValueArray.end(); jValueIter++)
        {
            auto jValue = *jValueIter;
            if (jValue.has_field("line"))
            {
                processLine(jValue.at("line"));
            }
            if (jValue.has_field("image"))
            {
                processImage(jValue.at("image"));
            }
            if (jValue.has_field("rotate"))
            {
                processRotate(jValue.at("rotate").as_integer());
            }
            if (jValue.has_field("circle"))
            {
                processCircle(jValue.at("circle"));
            }
            if (jValue.has_field("point"))
            {
                processPoint(jValue.at("point"));
            }
            if (jValue.has_field("rectangle"))
            {
                processRectangle(jValue.at("rectangle"));
            }
            if (jValue.has_field("string"))
            {
                processString(jValue.at("string"));
            }
        }
        return epaperRet::SUCCESS;
    }
};