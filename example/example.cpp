
#include "logger/boost_logger.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "epaper/epd7in5bpaint.hpp"
#include "epaper/imagedata.hpp"
#include <bcm2835.h>
#include "epaper/fonts.hpp"

int main(void)
{

    Paint paint(EPD_WIDTH, EPD_HEIGHT);

    if (paint.init() == -1)
    {
        return 0;
    }

    paint.Clear(EPDPAINT_WHITE);

    /* Draw something to the frame_buffer */
    /* For simplicity, the arguments are explicit numerical coordinates */
    paint.SetRotate(ROTATE_0);
    /* Write strings to the buffer */

    paint.DrawFilledRectangle(0, 6, 640, 30, EPDPAINT_RED);                               //x1,y1,x2,y2,Diagonal point
    paint.DrawStringAt(20, 10, "---Max's to do list---", &Font24, EPDPAINT_WHITE);        //x,y
    paint.DrawStringAt(20, 40, "1. support tafe sdh testing  ", &Font24, EPDPAINT_BLACK); //x,y
    paint.DrawStringAt(20, 70, "2. tech redar", &Font24, EPDPAINT_BLACK);                 //x,y
    paint.DrawStringAt(20, 100, "3. docker overlay2 sharing", &Font24, EPDPAINT_BLACK);   //x,y
    paint.DrawStringAt(20, 130, "4. ", &Font24, EPDPAINT_BLACK);                          //x,y
    paint.DrawFilledRectangle(0, 194, 640, 224, EPDPAINT_RED);
    paint.DrawStringAt(20, 200, "self study:", &Font24, EPDPAINT_WHITE);      //x,y
    paint.DrawStringAt(20, 230, "1. medis develop", &Font24, EPDPAINT_BLACK); //x,y
    paint.DrawStringAt(20, 260, "2. E73 develop", &Font24, EPDPAINT_BLACK);   //x,y
    paint.DrawStringAt(20, 290, "", &Font24, EPDPAINT_BLACK);                 //x,y
    paint.DrawStringAt(20, 320, "", &Font24, EPDPAINT_BLACK);                 //x,y

    paint.DisplayFrame();

    while (1)
        ;
    return 0;
}
