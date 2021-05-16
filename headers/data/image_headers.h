/*    ____         ________     __      _        ______  
 *   |  _ \_      |  ______|   |  \    | |     _/  ____\
 *   | | \_ \_    |  |         |   \   | |    /  _/   
 *   | |   \  |   |  |_____    | |\ \  | |   |  |   _____ 
 *   | |    | |   |  ______|   | | \ \ | |   |  |  |___  | 
 *   | |  _/ _|   |  |         | |  \ \| |    \  \    / /
 *   | |_/ _/     |  |_____    | |   \   |     \_ \__/ /
 *   |____/       |________|   |_|    \__|       \____/
 *               
 * __________________________________________________________
 * __________________________________________________________
 *                       Project DENG 
 *
 *
 * Copyright (C) 2020 - 2021
 * This Software is licensed under Apache License as described 
 * in the LICENSE.md file, which you should have recieved with 
 * this distribution.
 * 
 * You may reproduce and distribute copies of the
 * Work or Derivative Works thereof in any medium, with or without
 * modifications, and in Source or Object form, provided that You
 * meet the following conditions:
 *
 * (a) You must give any other recipients of the Work or
 *     Derivative Works a copy of this License; and
 *
 * (b) You must cause any modified files to carry prominent notices
 *     stating that You changed the files; and
 *
 * (c) You must retain, in the Source form of any Derivative Works
 *     that You distribute, all copyright, patent, trademark, and
 *     attribution notices from the Source form of the Work,
 *     excluding those notices that do not pertain to any part of
 *     the Derivative Works; and
 *
 * (d) If the Work includes a "NOTICE" text file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE text file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE text from the Work, provided
 *     that such additional attribution notices cannot be construed
 *     as modifying the License.
 *
 * You may add Your own copyright statement to Your modifications and
 * may provide additional or different license terms and conditions
 * for use, reproduction, or distribution of Your modifications, or
 * for any such Derivative Works as a whole, provided Your use,
 * reproduction, and distribution of the Work otherwise complies with
 * the conditions stated in this License.
 * ----------------------------------------------------------------
 * Purpose: Define structures for reading header info from simple 
 * bitmap image formats (tga and bmp)
 * Author: Karl-Mihkel Ott
 */ 

#ifndef __IMAGE_HEADERS_H
#define __IMAGE_HEADERS_H


/*******************************************/
/********** BMP Image Info Headers *********/
/*******************************************/
#define __BMPFileHeader_Size 14
typedef struct __BMPFileHeader {
    deng_ui16_t file_type;
    deng_ui32_t file_size;
    deng_ui16_t reserved1;
    deng_ui16_t reserved2;
    deng_ui32_t offset_data;
} __BMPFileHeader;


#define __BMPInfoHeader_Size    40
typedef struct __BMPInfoHeader {
    deng_ui32_t size;
    deng_i32_t width;
    deng_i32_t height;

    deng_ui16_t planes;
    deng_ui16_t bit_count;
    deng_ui32_t compression;
    deng_ui32_t size_image;
    deng_i32_t x_pixels_per_meter;
    deng_i32_t y_pixels_per_meter; 
    deng_ui32_t colors_used;
    deng_ui32_t colors_important;
} __BMPInfoHeader;


#define __BMPColorHeader_Size   24
typedef struct __BMPColorHeader {
    deng_ui32_t red_mask;
    deng_ui32_t green_mask;
    deng_ui32_t blue_mask;
    deng_ui32_t alpha_mask;
    deng_ui32_t color_space_type;
    deng_ui32_t unused[16];
} __BMPColorHeader;


/*******************************************/
/********** BMP Image Info Headers *********/
/*******************************************/
#define __TGATypeHeader_Size    3
typedef struct __TGATypeHeader {
    deng_ui8_t id_length;
    deng_ui8_t colormap_type;
    deng_ui8_t image_type;
} __TGATypeHeader;


#define __TGAColorMapHeader_Size    5
typedef struct __TGAColorMapHeader {
    deng_ui16_t first_colormap_index;
    deng_ui16_t colormap_length;
    deng_ui8_t entity_size;
} __TGAColorMapHeader;


#define __TGAInfoHeader_Size    10
typedef struct __TGAInfoHeader {
    deng_ui16_t x_origin;
    deng_ui16_t y_origin;
    deng_ui16_t width;
    deng_ui16_t height;
    deng_ui8_t bit_count;
    deng_ui8_t image_descriptor;
} __TGAInfoHeader;

#endif