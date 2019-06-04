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

class Paint
{
public:
    Paint(unsigned char *image, int width, int height)
    {
        this->rotate = ROTATE_0;
        this->image = image;
        /* 1 byte = 8 pixels, so the width should be the multiple of 8 */
        this->width = width % 8 ? width + 8 - (width % 8) : width;
        this->height = height;
    }

    void Clear(int color)
    {
        for (int x = 0; x < this->width; x++)
        {
            for (int y = 0; y < this->height; y++)
            {
                DrawAbsolutePixel(x, y, color);
            }
        }
    }
    int GetWidth(void)
    {
        return this->width;
    }
    void SetWidth(int width)
    {
        this->width = width % 8 ? width + 8 - (width % 8) : width;
    }

    int GetHeight(void)
    {
        return this->height;
    }
    void SetHeight(int height)
    {
        this->height = height;
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

    void DrawCharAt(int x, int y, char ascii_char, const sFONT *font, int color)
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

private:
    unsigned char *image;
    int width;
    int height;
    int rotate;
};
