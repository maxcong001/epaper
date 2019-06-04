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

const sFONT Font24 __attribute__((section("font"))) = {
	Font24_Table,
	17, /* Width */
	24, /* Height */
};
class fonts
{
public:
#if 0
	static constexpr sFONT Font8 = {
		font8Table::Font8_Table,
		5, /* Width */
		8, /* Height */
	};

	static constexpr sFONT Font12 = {
		font12Table::Font12_Table,
		7,  /* Width */
		12, /* Height */
	};

	static constexpr sFONT Font16 = {
		font16Table::Font16_Table,
		11, /* Width */
		16, /* Height */
	};

	static constexpr sFONT Font20 = {
		font20Table::Font20_Table,
		14, /* Width */
		20, /* Height */
	};

	static constexpr sFONT Font24 = {
		Font24_Table,
		17, /* Width */
		24, /* Height */
	};
#endif
};
