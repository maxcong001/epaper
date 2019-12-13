#pragma once

// Display orientation
#define ROTATE_0 0
#define ROTATE_90 1
#define ROTATE_180 2
#define ROTATE_270 3

// Color definition
#define EPDPAINT_BLACK 0x0
#define EPDPAINT_RED 0x4
#define EPDPAINT_WHITE 0x3

#include "fonts.hpp"
#include "Epd7in5bDriver.hpp"
#include <string.h>
#include <logger/logger.hpp>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

class Paint
{
public:
    Paint() : rotate(ROTATE_0), width(EPD_WIDTH), height(EPD_HEIGHT)
    {
    }
    ~Paint()
    {
        free(this->image);
    }

    int init()
    {
        this->image = (unsigned char *)malloc(this->width / 2 * (this->height));
        Clear(EPDPAINT_WHITE);
        return epd.Init();
    }

    static Paint *getInstance()
    {
        static Paint *ins = new Paint();
        return ins;
    }

    void Clear(int color = EPDPAINT_WHITE)
    {
        // to do
        switch (color)
        {
        case EPDPAINT_BLACK:
            memset(this->image, 0, this->width / 2 * (this->height));
            break;
        case EPDPAINT_RED:
            memset(this->image, 0x4444, this->width / 2 * (this->height));
            break;
        case EPDPAINT_WHITE:
            memset(this->image, 0x30333, this->width / 2 * (this->height));
            break;
        default:
            /* do nothing */
            break;
        }
    }
    int GetWidth(void)
    {
        return this->width;
    }

    int GetHeight(void)
    {
        return this->height;
    }
    int GetRotate(void)
    {
        return this->rotate;
    }

    void SetRotate(int rotate)
    {
        this->rotate = rotate;
    }
    unsigned char *GetImage(void)
    {
        return this->image;
    }
    void SetImage(unsigned char *imageSrc)
    {
        memcpy(this->image, imageSrc, this->width / 2 * (this->height));
    }
    void DrawAbsolutePixel(int x, int y, int color)
    {
        if (x < 0 || x >= this->width || y < 0 || y >= this->height)
        {
            return;
        }
        switch (color)
        {
        case EPDPAINT_BLACK:
            image[(x + y * this->width) / 2] &= ~(0xF0 >> (x % 2 * 4));
            break;
        case EPDPAINT_RED:
            image[(x + y * this->width) / 2] &= ~(0xF0 >> (x % 2 * 4));
            image[(x + y * this->width) / 2] ^= 0x40 >> (x % 2 * 4);
            break;
        case EPDPAINT_WHITE:
            image[(x + y * this->width) / 2] &= ~(0xF0 >> (x % 2 * 4));
            image[(x + y * this->width) / 2] ^= 0x30 >> (x % 2 * 4);
            break;
        default:
            /* do nothing */
            break;
        }
    }
    int getMaxAbsWidth()
    {
        if (this->rotate == ROTATE_0 || this->rotate == ROTATE_180)
        {
            return this->width;
        }
        else if (this->rotate == ROTATE_90 || this->rotate == ROTATE_270)
        {
            return this->height;
        }
        else
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupported rotate, rotate is : " << this->rotate);
            }
            return 0;
        }
    }
    int getMaxAbsHeight()
    {
        if (this->rotate == ROTATE_0 || this->rotate == ROTATE_180)
        {
            return this->height;
        }
        else if (this->rotate == ROTATE_90 || this->rotate == ROTATE_270)
        {
            return this->width;
        }
        else
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "unsupported rotate, rotate is : " << this->rotate);
            }
            return 0;
        }
    }
    void DrawPixel(int x, int y, int color)
    {
        int point_temp;

        if (this->rotate == ROTATE_0)
        {
            if (x < 0 || x >= this->width || y < 0 || y >= this->height)
            {
                return;
            }
            DrawAbsolutePixel(x, y, color);
        }
        else if (this->rotate == ROTATE_90)
        {
            if (x < 0 || x >= this->height || y < 0 || y >= this->width)
            {
                return;
            }
            point_temp = x;
            x = this->width - y;
            y = point_temp;
            DrawAbsolutePixel(x, y, color);
        }
        else if (this->rotate == ROTATE_180)
        {
            if (x < 0 || x >= this->width || y < 0 || y >= this->height)
            {
                return;
            }
            x = this->width - x;
            y = this->height - y;
            DrawAbsolutePixel(x, y, color);
        }
        else if (this->rotate == ROTATE_270)
        {
            if (x < 0 || x >= this->height || y < 0 || y >= this->width)
            {
                return;
            }
            point_temp = x;
            x = y;
            y = this->height - point_temp;
            DrawAbsolutePixel(x, y, color);
        }
    }

    void DrawCharAt(int x, int y, char ascii_char, const sFONT *font, int color, int bcolour = EPDPAINT_WHITE)
    {
        int i, j;
        unsigned int char_offset = (ascii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
        const unsigned char *ptr = &font->table[char_offset];

        for (j = 0; j < font->Height; j++)
        {
            for (i = 0; i < font->Width; i++)
            {
                if (*ptr & (0x80 >> (i % 8)))
                {
                    DrawPixel(x + i, y + j, color);
                }
                if (i % 8 == 7)
                {
                    ptr++;
                }
            }
            if (font->Width % 8 != 0)
            {
                ptr++;
            }
        }
    }

    void DrawStringAt(int x, int y, const char *text, const sFONT *font, int color)
    {
        const char *p_text = text;
        unsigned int counter = 0;
        int refcolumn = x;

        /* Send the string character by character on EPD */
        while (*p_text != 0)
        {
            /* Display one character on EPD */
            DrawCharAt(refcolumn, y, *p_text, font, color);
            /* Decrement the column position by 16 */
            refcolumn += font->Width;
            /* Point on the next character */
            p_text++;
            counter++;
        }
    }
    void DrawLine(int x0, int y0, int x1, int y1, int color)
    {
        /* Bresenham algorithm */
        int dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
        int sx = x0 < x1 ? 1 : -1;
        int dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while ((x0 != x1) && (y0 != y1))
        {
            DrawPixel(x0, y0, color);
            if (2 * err >= dy)
            {
                err += dy;
                x0 += sx;
            }
            if (2 * err <= dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }
    void DrawHorizontalLine(int x, int y, int line_width, int color)
    {
        int i;
        for (i = x; i < x + line_width; i++)
        {
            DrawPixel(i, y, color);
        }
    }
    void DrawVerticalLine(int x, int y, int line_height, int color)
    {
        int i;
        for (i = y; i < y + line_height; i++)
        {
            DrawPixel(x, i, color);
        }
    }
    void DrawRectangle(int x0, int y0, int x1, int y1, int color)
    {
        int min_x, min_y, max_x, max_y;
        min_x = x1 > x0 ? x0 : x1;
        max_x = x1 > x0 ? x1 : x0;
        min_y = y1 > y0 ? y0 : y1;
        max_y = y1 > y0 ? y1 : y0;

        DrawHorizontalLine(min_x, min_y, max_x - min_x + 1, color);
        DrawHorizontalLine(min_x, max_y, max_x - min_x + 1, color);
        DrawVerticalLine(min_x, min_y, max_y - min_y + 1, color);
        DrawVerticalLine(max_x, min_y, max_y - min_y + 1, color);
    }
    void DrawFilledRectangle(int x0, int y0, int x1, int y1, int color)
    {
        int min_x, min_y, max_x, max_y;
        int i;
        min_x = x1 > x0 ? x0 : x1;
        max_x = x1 > x0 ? x1 : x0;
        min_y = y1 > y0 ? y0 : y1;
        max_y = y1 > y0 ? y1 : y0;

        for (i = min_x; i <= max_x; i++)
        {
            DrawVerticalLine(i, min_y, max_y - min_y + 1, color);
        }
    }
    void DrawCircle(int x, int y, int radius, int color)
    {
        /* Bresenham algorithm */
        int x_pos = -radius;
        int y_pos = 0;
        int err = 2 - 2 * radius;
        int e2;

        do
        {
            DrawPixel(x - x_pos, y + y_pos, color);
            DrawPixel(x + x_pos, y + y_pos, color);
            DrawPixel(x + x_pos, y - y_pos, color);
            DrawPixel(x - x_pos, y - y_pos, color);
            e2 = err;
            if (e2 <= y_pos)
            {
                err += ++y_pos * 2 + 1;
                if (-x_pos == y_pos && e2 <= x_pos)
                {
                    e2 = 0;
                }
            }
            if (e2 > x_pos)
            {
                err += ++x_pos * 2 + 1;
            }
        } while (x_pos <= 0);
    }
    void DrawFilledCircle(int x, int y, int radius, int color)
    {
        /* Bresenham algorithm */
        int x_pos = -radius;
        int y_pos = 0;
        int err = 2 - 2 * radius;
        int e2;

        do
        {
            DrawPixel(x - x_pos, y + y_pos, color);
            DrawPixel(x + x_pos, y + y_pos, color);
            DrawPixel(x + x_pos, y - y_pos, color);
            DrawPixel(x - x_pos, y - y_pos, color);
            DrawHorizontalLine(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, color);
            DrawHorizontalLine(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, color);
            e2 = err;
            if (e2 <= y_pos)
            {
                err += ++y_pos * 2 + 1;
                if (-x_pos == y_pos && e2 <= x_pos)
                {
                    e2 = 0;
                }
            }
            if (e2 > x_pos)
            {
                err += ++x_pos * 2 + 1;
            }
        } while (x_pos <= 0);
    }
    void DrawPicture()
    {
    }

    void DisplayFrame()
    {
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "now display frame");
        }
        epd.DisplayFrame1(GetImage());
    }

    int do_convert(iconv_t cd, const char *from, size_t from_size, std::string *to)
    {
        char *in_buf_ptr = const_cast<char *>(from);
        size_t in_bytes_left = from_size;
        size_t out_bytes = in_bytes_left * 3 + 1;
        size_t out_bytes_left = out_bytes;
        std::string out(out_bytes_left, '\0');
        char *out_buf_start = const_cast<char *>(out.c_str());
        char *out_buf_ptr = out_buf_start;

        int bytes = iconv(cd, &in_buf_ptr, &in_bytes_left, &out_buf_ptr, &out_bytes_left);
        if (-1 == bytes)
            return errno;

        to->assign(out_buf_start, out_bytes - out_bytes_left);
        return 0;
    }

    std::string convert(const std::string &from_charset, const std::string &to_charset,
                        const std::string &from,
                        bool ignore_error, bool skip_error)
    {
        std::string result;
        char *in_buf = const_cast<char *>(from.c_str());
        size_t in_bytes = from.size();
        size_t in_bytes_left = in_bytes;
        iconv_t cd = iconv_open(to_charset.c_str(), from_charset.c_str());

        if ((iconv_t)(-1) == cd)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "iconv_open return error, error number is : " << errno);
            }
            return "";
        }
        while (in_bytes_left > 0)
        {
            int errcode;
            size_t out_bytes = in_bytes_left * 3 + 1;
            size_t out_bytes_left = out_bytes;
            std::string out(out_bytes_left, '\0');
            char *out_buf = const_cast<char *>(out.c_str());
            char *out_buf_start = out_buf;
            char *in_buf_start = in_buf;

            int bytes = iconv(cd, &in_buf, &in_bytes_left, &out_buf, &out_bytes_left);
            if (bytes != -1)
            {
                result.append(out_buf_start, out_bytes - out_bytes_left);
                break;
            }
            else if (!ignore_error)
            {
                errcode = errno;
                iconv_close(cd);
                if (CHECK_LOG_LEVEL(debug))
                {
                    __LOG(debug, "iconv return error, error number is : " << errno);
                }
                return result;
            }
            else
            {
                // EILSEQ An invalid multibyte sequence has been encountered in the input.
                // EINVAL An incomplete multibyte sequence has been encountered in the input.
                if ((errno != EINVAL) &&
                    (errno != EILSEQ))
                {
                    // E2BIG  There is not sufficient room at *outbuf.
                    errcode = errno;
                    iconv_close(cd);
                    if (CHECK_LOG_LEVEL(debug))
                    {
                        __LOG(debug, "iconv return error, error number is : " << errno);
                    }
                    return result;
                }
                else
                {

                    if (in_buf != in_buf_start)
                    {
                        std::string str;
                        errcode = do_convert(cd, in_buf_start, in_buf - in_buf_start, &str);
                        if (errcode != 0)
                        {
                            iconv_close(cd);
                            if (CHECK_LOG_LEVEL(debug))
                            {
                                __LOG(debug, "do_convert return error, error number is : " << errno);
                            }
                            return result;
                        }

                        result.append(str);
                    }

                    if (!skip_error)
                    {
                        result.append(in_buf, 1);
                    }

                    --in_bytes_left;
                    ++in_buf;
                }
            }
        }

        if (-1 == iconv_close(cd))
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "iconv_close return error, error number is : " << errno);
            }
            return result;
        }

        return result;
    }

    std::string utf8_to_gb2312(const std::string &from, bool ignore_error = false, bool skip_error = false)
    {
        return convert("utf-8", "gb2312", from, ignore_error, skip_error);
    }

    void printString(std::string inStr, int font, int posx, int posy, int colour, int bcolour)
    {
        std::string outStr = utf8_to_gb2312(inStr);

        for (unsigned int i = 0; i < outStr.length();)
        {
            if (outStr[i] <= 0x7F)
            {
                printf("receive ASCII code : %c\n", outStr[i]);
                if (font == 8)
                {
                    display_word(&outStr[i], &Font8, false, posx, posy, colour, bcolour);
                }
                else if (font == 12)
                {
                    display_word(&outStr[i], &Font12, false, posx, posy, colour, bcolour);
                }
                else if (font == 16)
                {
                    display_word(&outStr[i], &Font16, false, posx, posy, colour, bcolour);
                }
                else if (font == 20)
                {
                    display_word(&outStr[i], &Font20, false, posx, posy, colour, bcolour);
                }
                else if (font == 24)
                {
                    display_word(&outStr[i], &Font24, false, posx, posy, colour, bcolour);
                }
                else
                {
                    if (CHECK_LOG_LEVEL(debug))
                    {
                        __LOG(debug, "unsupported font, font is : " << font);
                    }
                    i++;
                    continue;
                }
                i++;
            }
            else
            {
                if (font == 12)
                {
                    display_word(&outStr[i], &HZFont12, true, posx, posy, colour, bcolour);
                }
                else if (font == 14)
                {
                    display_word(&outStr[i], &HZFont14, true, posx, posy, colour, bcolour);
                }
                else if (font == 16)
                {
                    display_word(&outStr[i], &HZFont16, true, posx, posy, colour, bcolour);
                }
                else if (font == 24)
                {
                    display_word(&outStr[i], &HZFont24, true, posx, posy, colour, bcolour);
                }
                else if (font == 32)
                {
                    display_word(&outStr[i], &HZFont32, true, posx, posy, colour, bcolour);
                }
                else if (font == 40)
                {
                    display_word(&outStr[i], &HZFont40, true, posx, posy, colour, bcolour);
                }
                else if (font == 48)
                {
                    display_word(&outStr[i], &HZFont48, true, posx, posy, colour, bcolour);
                }
                else
                {
                    if (CHECK_LOG_LEVEL(debug))
                    {
                        __LOG(debug, "unsupported font, font is : " << font);
                    }

                    i += 2;
                    continue;
                }
                i += 2;
            }
        }
    }

    void display_word(char *oneChar, const sFONT *font, bool isCH, int &posx, int &posy, int colour, int bcolour)
    {
        if ((posy + font->Height) <= getMaxAbsHeight())
        {
            if ((posx + font->Width) <= getMaxAbsWidth())
            {
                if (isCH)
                {
                    Bytes_Display(oneChar, font, posx, posy, colour, bcolour);
                }
                else
                {
                    Paint::getInstance()->DrawCharAt(posx, posy, *oneChar, font, colour, bcolour);
                }
                posx += font->Width;
            }
            else
            {

                if ((posy + 2 * font->Height) <= getMaxAbsHeight())
                {
                    posx = 0;
                    posy += font->Height;
                    if (isCH)
                    {
                        Bytes_Display(oneChar, font, posx, posy, colour, bcolour);
                    }
                    else
                    {
                        Paint::getInstance()->DrawCharAt(posx, posy, *oneChar, font, colour, bcolour);
                    }
                    posx += font->Width;
                }
                else
                {
                    return;
                }
            }
        }
        else
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "out of boundry");
            }
        }
    }

    void Bytes_Read_from_HZK(unsigned char *s, char *const chs, const sFONT *font)
    {
        FILE *fp;
        unsigned long offset;
        std::string fileName = "HZK" + std::to_string(font->Height);
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "open HZK lib with name : " << fileName);
        }
        if (font->Height == 12)
        {
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 24;
        }
        else if (font->Height == 14)
        {
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 28;
        }
        else if (font->Height == 16)
        {
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 32;
        }
        else if (font->Height == 24)
        {
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 72;
        }
        else if (font->Height == 32)
        {
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 128;
        }
        else if (font->Height == 40)
        {
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 200;
        }
        else if (font->Height == 48)
        {
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 288;
        }
        else
        {
            // default 16
            offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 32;
        }

        if ((fp = fopen(fileName.c_str(), "r")) == NULL)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "open " << fileName << " fail");
            }
            return;
        }
        fseek(fp, offset, SEEK_SET);
        fread(chs, ((font->Height) / 8 + 1) * (font->Width), 1, fp);
        fclose(fp);
    }

    void Bytes_Display(char *const s, const sFONT *font, int positionx, int positiony, int fcolour, int bcolour)
    {
        char *chs = (char *)malloc(((font->Width) / 8 + 1) * (font->Height));
        Bytes_Read_from_HZK((unsigned char *)(s), chs, font);

        int charPos = 0;
        for (uint16_t i = 0; i < (font->Height); i++)
        {
            for (uint16_t j = 0; j < (font->Height); j++)
            {
                char tmpChar = chs[charPos + j / 8];
                if (tmpChar & (0x1 << (7 - (j % 8))))
                {
                    DrawPixel(positionx + j, positiony + i, fcolour);
                }
                else
                {
                    DrawPixel(positionx + j, positiony + i, bcolour);
                }
            }
            charPos += (font->Width / 8);
        }
        free(chs);
    }

private:
    Epd7in5bDriver epd;
    unsigned char *image;
    int rotate;
    int width;
    int height;
};
