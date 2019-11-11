// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().  Pass NULL to
// revert to 'classic' fixed-space bitmap font.

#ifndef _GFXFONT_H_
#define _GFXFONT_H_

/// Font data stored PER GLYPH
typedef struct {
	uint16_t bitmapOffset;     ///< Pointer into GFXfont->bitmap
	uint8_t  width;            ///< Bitmap dimensions in pixels
        uint8_t  height;           ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;         ///< Distance to advance cursor (x axis)
	int8_t   xOffset;          ///< X dist from cursor pos to UL corner
        int8_t   yOffset;          ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct { 
	uint8_t  *bitmap;      ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;       ///< Glyph array
	uint8_t   first;       ///< ASCII extents (first char)
        uint8_t   last;        ///< ASCII extents (last char)
	uint8_t   yAdvance;    ///< Newline distance (y axis)

	//wucy addition
	constexpr uint8_t charMaxHeight() {

		uint8_t maxHeight = 0;

		if(glyph == NULL) {

			return 8;

		}
		else {

			for(uint16_t p = 0; p <= last - first; p++) {

				if ((glyph + p)->height > maxHeight) {

					maxHeight = (glyph + p)->height;

				}
			}
		}
		return maxHeight;
	};

	constexpr uint8_t charMaxWidth() {

		uint8_t maxWidth = 0;

		if(glyph == NULL) {

			return 6;

		}
		else {

			for(uint16_t p = 0; p <= last - first; p++) {

				if ((glyph + p)->width > maxWidth) {

					maxWidth = (glyph + p)->width;

				}
			}
			return maxWidth;
		}
	};

} GFXfont;

#endif // _GFXFONT_H_
