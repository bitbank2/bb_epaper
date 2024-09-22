//
// C structures for defining a BB_FONT file
// Written by Larry Bank
// Copyright (c) 2024 BitBank Software, Inc.
//
// Use of this software is governed by the Business Source License
// included in the file ./LICENSE.
//
// As of the Change Date specified in that file, in accordance with
// the Business Source License, use of this software will be governed
// by the Apache License, Version 2.0, included in the file
// ./APL.txt.
//
// The BB_FONT is at the start of the file
// followed by BB_GLYPH structures for each character
// and finally the packed bitmap data (one after another)
// is at the end of the file
//

#ifndef _BB_FONT_H_
#define _BB_FONT_H_

// 16-bit marker at the start of a BB_FONT file
// (BitBank FontFile)
#define BB_MARKER 0xBBFF
// Font info per character (glyph)
typedef struct {
  uint16_t bitmapOffset; // Offset to compressed bitmap data for this glyph (starting from the end of the BB_GLYPH[] array)
  uint8_t width;         // bitmap width in pixels
  uint8_t xAdvance;      // total width in pixels (bitmap + padding)
  uint16_t height;        // bitmap height in pixels
  int16_t xOffset;        // left padding to upper left corner
  int16_t yOffset;        // padding from baseline to upper left corner (usually negative)
} BB_GLYPH;

// This structure is stored at the beginning of a BB_FONT file
typedef struct {
  uint16_t u16Marker; // 16-bit Marker defining a BB_FONT file
  uint16_t first;      // first char (ASCII value)
  uint16_t last;       // last char (ASCII value)
  uint16_t height;    // total height of font
  uint32_t rotation; // store this as 32-bits to not have a struct packing problem
  BB_GLYPH glyphs[];  // Array of glyphs (one for each char)
} BB_FONT;

#endif // _BB_FONT_H_
