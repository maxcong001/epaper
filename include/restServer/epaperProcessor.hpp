#pragma once
#include <cpprest/http_msg.h>
#include <logger/logger.hpp>
#include "epaper/epd7in5bpaint.hpp"
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
    static epaperRet processImage(web::json::value jValue)
    {
        auto imageBase64 = jValue.at("image").as_string();
        auto image = utility::conversions::from_base64(imageBase64);
        if (image.size() != EPD_WIDTH * EPD_HEIGHT / 2)
        {
            return epaperRet::BAD_REQUEST;
        }

        unsigned char *imageData = (unsigned char *)malloc(EPD_WIDTH * EPD_HEIGHT / 2);
        for (int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 2; i++)
        {
            imageData[i] = image[i];
        }
        Paint::getInstance()->SetImage(imageData);
        free(imageData);
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
    static printString(string inStr, int font)
    {
        if (font == 24)
        {
            for (int i = 0; i < inStr.length(); i++)
            {
                if (inStr[i] <= 0x7F)
                {
                    // ASCII
                    if (CHECK_LOG_LEVEL(debug))
                    {
                        __LOG(debug, "receive ASCII char :" + inStr[i]);
                    }
                }
                else
                {
                    char chs[32];
                    Bytes_Read_from_HZK16((unsigned char *)out, chs);
                    Bytes_Display(chs);
                }
            }
        }
    }
    static void Bytes_Read_from_HZK16(unsigned char *s, char *const chs)
    {
        FILE *fp;
        unsigned long offset;

        offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 32; //根据内码找出汉字在HZK16�?的偏移位�?

        if ((fp = fopen("HZK16", "r")) == NULL)
            return;                  //打开字库文件
        fseek(fp, offset, SEEK_SET); //文件指针偏移到�?�找的汉字�??
        fread(chs, 32, 1, fp);       //读取该汉字的字模
        fclose(fp);
    }
    static void Bytes_Display(char *const chs)
    {
        int i, j;

        for (i = 0; i < 32; i++) //显示
        {
            if (i % 2 == 0)
                printf("\n");
            for (j = 7; j >= 0; j--)
            {
                if (chs[i] & (0x1 << j))
                {
                    printf("O");
                }
                else
                {
                    printf("-");
                }
            }
        }
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

        json::array pos = jValue.at("position").as_array();

        if (pos.size() != 2)
        {
            return epaperRet::BAD_REQUEST;
        }
        int posx = pos.at(0).as_integer();
        int posy = pos.at(1).as_integer();

        auto content = jValue.at("content").as_string();

        int font = jValue.at("font").as_integer();
        if (font == 8)
        {
            Paint::getInstance()->DrawStringAt(posx, posy, content.c_str(), &Font8, colour);
        }
        else if (font == 12)
        {
            Paint::getInstance()->DrawStringAt(posx, posy, content.c_str(), &Font12, colour);
        }
        else if (font == 16)
        {
            Paint::getInstance()->DrawStringAt(posx, posy, content.c_str(), &Font16, colour);
        }
        else if (font == 20)
        {
            Paint::getInstance()->DrawStringAt(posx, posy, content.c_str(), &Font20, colour);
        }
        else if (font == 24)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "receive content : " << content);
            }
            printString(content, 24);

            //Paint::getInstance()->DrawStringAt(posx, posy, content.c_str(), &Font24, colour);
        }
        else
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupported font, font is : " << font);
            }
            return epaperRet::BAD_REQUEST;
        }
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