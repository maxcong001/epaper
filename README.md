# epaper

## driver open API

1. void Clear(int color)
2. void DrawCharAt(int x, int y, char ascii_char, const sFONT *font, int color)    
note: need to change this API to font size enum.(8-12-16-20-24)
3. void DrawCircle(int x, int y, int radius, int color)
4. void DrawFilledCircle(int x, int y, int radius, int color)
5. void DrawFilledRectangle(int x0, int y0, int x1, int y1, int color)
6. void DrawRectangle(int x0, int y0, int x1, int y1, int color)
7. void DrawLine(int x0, int y0, int x1, int y1, int color)
8. void DrawStringAt(int x, int y, const char *text, const sFONT *font, int color)
9.  void SetRotate(int rotate)

10. 
11. unsigned char *GetImage(void)
12. void DisplayFrame1(const unsigned char *frame_buffer)

enjoy

Max
