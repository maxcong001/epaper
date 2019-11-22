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
        epd.DisplayFrame1(GetImage());
    }

    // 传递给do_convert的in_buf，所有字节数（in_buf_size指定）都是可以转换成功的
    static int do_convert(iconv_t cd, const char *from, size_t from_size, std::string *to)
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
    // 可忽略不能转换的部分，
    // 也可以在结果中保留不能被转换的部分
    static std::string convert(const std::string &from_charset, const std::string &to_charset,
                               const std::string &from,
                               bool ignore_error, bool skip_error)
    {
        std::string result; // 用来保存处理后的内容
        char *in_buf = const_cast<char *>(from.c_str());
        size_t in_bytes = from.size();   // 需要处理的总字节数
        size_t in_bytes_left = in_bytes; // 剩余的未被处理的字节数
        iconv_t cd = iconv_open(to_charset.c_str(), from_charset.c_str());

        if ((iconv_t)(-1) == cd)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "iconv_open return error, error number is : " + errno);
            }
            return "";
        }
        while (in_bytes_left > 0)
        {
            int errcode;
            size_t out_bytes = in_bytes_left * 3 + 1; // 保证足够大
            size_t out_bytes_left = out_bytes;
            std::string out(out_bytes_left, '\0');
            char *out_buf = const_cast<char *>(out.c_str());
            char *out_buf_start = out_buf;
            char *in_buf_start = in_buf;

            // 如果成功，返回值bytes为0
            // 如果成功，in_buf指向in的结尾符，即'\0'，同时in_bytes_left值为0
            // 如果失败，in_buf指向未能转换的起始地址，而in_bytes_left值为剩余的未被转换的（可能含有可转换的）字节数
            // 如果成功，则out_bytes-out_bytes_left值为转换后的字节数
            // 如果成功，则out_buf_start存储了被转换后的结果，有效长度为out_bytes-out_bytes_left
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
                    __LOG(debug, "iconv return error, error number is : " + errno);
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
                        __LOG(debug, "iconv return error, error number is : " + errno);
                    }
                    return result;
                }
                else
                {
                    // in_buf之前部分是可以转换的
                    if (in_buf != in_buf_start)
                    {
                        std::string str;
                        errcode = do_convert(cd, in_buf_start, in_buf - in_buf_start, &str);
                        if (errcode != 0)
                        {
                            iconv_close(cd);
                            if (CHECK_LOG_LEVEL(debug))
                            {
                                __LOG(debug, "do_convert return error, error number is : " + errno);
                            }
                            return result;
                        }

                        result.append(str);
                    }

                    // skip_error决定未能被转换的是否出现在结果当中
                    if (!skip_error)
                    {
                        result.append(in_buf, 1);
                    }

                    // 往前推进
                    --in_bytes_left; // 将导致while语句结束
                    ++in_buf;
                }
            }
        }

        if (-1 == iconv_close(cd))
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "iconv_close return error, error number is : " + errno);
            }
            return result;
        }

        return result;
    }

    static std::string utf8_to_gb2312(const std::string &from, bool ignore_error = false, bool skip_error = false)
    {
        return convert("utf-8", "gb2312", from, ignore_error, skip_error);
    }

    static void printString(std::string inStr, int font, int positionx, int positiony, int colour, int bcolour)
    {
        //char out_array[256];
        //u2g(const_cast<char *>(inStr.c_str()), inStr.length(), out_array, 256);
        std::string outStr = utf8_to_gb2312(inStr);

        for (int i = 0; i < outStr.length();)
        {
            if (outStr[i] <= 0x7F)
            {
                printf("receive ASCII code : %c\n", outStr[i]);
                if (font == 8)
                {
                    display_word(Font8, false, posx, posy, colour, bcolour);
                }
                else if (font == 12)
                {
                    display_word(Font12, false, posx, posy, colour, bcolour);
                }
                else if (font == 16)
                {
                    display_word(Font16, false, posx, posy, colour, bcolour);
                }
                else if (font == 20)
                {
                    display_word(Font20, false, posx, posy, colour, bcolour);
                }
                else if (font == 24)
                {
                    display_word(Font24, false, posx, posy, colour, bcolour);
                }
                else
                {
                    if (CHECK_LOG_LEVEL(debug))
                    {
                        __LOG(debug, "unsupported font, font is : " << font);
                    }
                    return epaperRet::BAD_REQUEST;
                }
                i++;
            }
            else
            {
                if (font == 12)
                {
                    display_word(HZFont12, true, posx, posy, colour, bcolour);
                }
                else if (font == 14)
                {
                    display_word(HZFont14, true, posx, posy, colour, bcolour);
                }
                else if (font == 16)
                {
                    display_word(HZFont16, true, posx, posy, colour, bcolour);
                }
                else if (font == 20)
                {
                    display_word(HZFont20, true, posx, posy, colour, bcolour);
                }
                else if (font == 24)
                {
                    display_word(HZFont24, true, posx, posy, colour, bcolour);
                }
                else if (font == 32)
                {
                    display_word(HZFont32, true, posx, posy, colour, bcolour);
                }
                else if (font == 40)
                {
                    display_word(HZFont40, true, posx, posy, colour, bcolour);
                }
                else if (font == 48)
                {
                    display_word(HZFont48, true, posx, posy, colour, bcolour);
                }
                else
                {
                    if (CHECK_LOG_LEVEL(debug))
                    {
                        __LOG(debug, "unsupported font, font is : " << font);
                    }
                    return epaperRet::BAD_REQUEST;
                }

                i += 2;
            }
        }
    }

    static void display_word(const sFONT *font, bool isCH, int &posx, int &posy, int colour, int bcolour)
    {
        if (((posx + font.Width) <= getMaxAbsWidth()) && ((posy + font.Height) <= getMaxAbsHeight()))
        {
            if (isCH)
            {
                Bytes_Display(&outStr[i], &font, posx, posy, colour, colour, bcolour);
            }
            else
            {
                Paint::getInstance()->DrawCharAt(posx, posy, outStr[i], font, colour, bcolour);
            }
            posx += font.Width;
            posy + = font.Height;
        }
        else
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "out of boundry");
            }
        }
    }
    static void Bytes_Read_from_HZK(unsigned char *s, char *const chs, const sFONT *font)
    {
        FILE *fp;
        unsigned long offset;

        std::string fileName = "HZK" + font->Height;

        offset = ((s[0] - 0xa1) * 94 + (s[1] - 0xa1)) * 32; //根据内码找出汉字在HZK16�?的偏移位�?

        if ((fp = fopen(fileName.c_str(), "r")) == NULL)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "open " + fileName " fail");
            }
            return;
        }                                                      //打开字库文件
        fseek(fp, offset, SEEK_SET);                           //文件指针偏移到�?�找的汉字�??
        fread(chs, (font->Height) * (font->Width) / 8, 1, fp); //读取该汉字的字模
        fclose(fp);
    }
    static void Bytes_Display(char *const s, const sFONT *font, int positionx, int positiony, int fcolour, int bcolour)
    {

        std::string sstring;
        sstring.reserve((font->Width) * (font->Height) / 8);
        char *chs = const_case<char *>(sstring.c_str());

        Bytes_Read_from_HZK((unsigned char *)(s), chs, font);

        int i, j;
        int charPos = 0;
        for (i = 0; i < font->Height; i++)
        {
            for (j = 0; j < font->Height; j++)
            {
                char tmpChar = chs[charPos + j / 8];
                if (tmpChar & (0x1 << (-(j % 8) + 8))
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
    }

private:
    Epd7in5bDriver epd;
    unsigned char *image;
    int rotate;
    int width;
    int height;
};
