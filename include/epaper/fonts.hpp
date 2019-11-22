#pragma once

/* Max size of bitmap will based on a font24 (17x24) */
#define MAX_HEIGHT_FONT 24
#define MAX_WIDTH_FONT 17
#define OFFSET_BITMAP 54

#include <stdint.h>
#include "epaper/font8.hpp"
#include "epaper/font12.hpp"
#include "epaper/font16.hpp"
#include "epaper/font20.hpp"
#include "epaper/font24.hpp"
typedef struct _tFont
{
	const uint8_t *table;
	uint16_t Width;
	uint16_t Height;

} sFONT;

const sFONT Font8 __attribute__((section("font"))) = {
	Font8_Table,
	5, /* Width */
	8, /* Height */
};

const sFONT Font12 __attribute__((section("font"))) = {
	Font12_Table,
	7,  /* Width */
	12, /* Height */
};

const sFONT Font16 __attribute__((section("font"))) = {
	Font16_Table,
	11, /* Width */
	16, /* Height */
};

const sFONT Font20 __attribute__((section("font"))) = {
	Font20_Table,
	14, /* Width */
	20, /* Height */
};

const sFONT Font24 __attribute__((section("font"))) = {
	Font24_Table,
	17, /* Width */
	24, /* Height */
};

const sFONT HZFont12 __attribute__((section("font"))) = {
	Font12_Table,
	16, /* Width */
	12, /* Height */
};

const sFONT HZFont14 __attribute__((section("font"))) = {
	Font12_Table,
	16, /* Width */
	14, /* Height */
};

const sFONT HZFont16 __attribute__((section("font"))) = {
	Font12_Table,
	16, /* Width */
	16, /* Height */
};

const sFONT HZFont24 __attribute__((section("font"))) = {
	Font12_Table,
	24, /* Width */
	24, /* Height */
};

const sFONT HZFont32 __attribute__((section("font"))) = {
	Font12_Table,
	32, /* Width */
	32, /* Height */
};

const sFONT HZFont40 __attribute__((section("font"))) = {
	Font12_Table,
	40, /* Width */
	40, /* Height */
};

const sFONT HZFont48 __attribute__((section("font"))) = {
	Font12_Table,
	Font8_Table,
	48, /* Width */
	48, /* Height */
};
