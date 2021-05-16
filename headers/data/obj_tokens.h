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
 */ 


#ifndef __OBJ_TOKENS_H
#define __OBJ_TOKENS_H


#ifdef __OBJ_TOKENS_C
    #include <stdlib.h>
    #include <string.h>   

    #include <common/base_types.h>   
    #include <common/common.h>
    #include <common/hashmap.h>
#endif


typedef enum __das_WavefrontObjSpecType {
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_NONE            = -1,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_OBJ_DECL        = 0,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_GROUP_DECL      = 1,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_DECL       = 2,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_TEX_DECL   = 3,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_NORM_DECL  = 4,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_PARAM_DECL = 5,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_FACE_DECL       = 6,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_POL_LINE_DECL   = 7,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_MTL_INCL        = 8,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_MTL_USE         = 9,
    DAS_WAVEFRONT_OBJ_SPEC_TYPE_SHADING_SPEC    = 10
} __das_WavefrontObjSpecType;


/*
 * Structure for defining all statements with their keywords and valid
 * object types that could be used
 */
typedef struct __das_WavefrontObjStatement {
    char *keyword;
    deng_i32_t min_obj_c;
    deng_i32_t max_obj_c;
    __das_WavefrontObjSpecType spec_type;
} __das_WavefrontObjStatement;


#ifdef __OBJ_TOKENS_C
    /// Map for containing data about all possible statements
    static Hashmap __statement_map;

    /// Array with all statement specifications
    static __das_WavefrontObjStatement __statements[] = {
        { "o", 0, 1, DAS_WAVEFRONT_OBJ_SPEC_TYPE_OBJ_DECL },
        { "g", 0, 1, DAS_WAVEFRONT_OBJ_SPEC_TYPE_GROUP_DECL },
        { "v", 3, 4, DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_DECL },
        { "vt", 1, 3, DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_TEX_DECL },
        { "vn", 3, 3, DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_NORM_DECL },
        { "vp", 1, 3, DAS_WAVEFRONT_OBJ_SPEC_TYPE_VERT_PARAM_DECL },
        { "f", 3, 4, DAS_WAVEFRONT_OBJ_SPEC_TYPE_FACE_DECL },
        { "l", 1, UINT32_MAX, DAS_WAVEFRONT_OBJ_SPEC_TYPE_POL_LINE_DECL },
        { "mtllib", 1, 1, DAS_WAVEFRONT_OBJ_SPEC_TYPE_MTL_INCL },
        { "usemtl", 1, 1, DAS_WAVEFRONT_OBJ_SPEC_TYPE_MTL_USE },
        { "s", 1, 1, DAS_WAVEFRONT_OBJ_SPEC_TYPE_SHADING_SPEC }
    };
#endif


/*
 * Create a hashmap out of all statements that can be accessed with
 * keywords
 */
void das_WavefrontObjTokenise();


/*
 * Free all memory that was used by tokens
 */
void das_WavefrontObjUntokenise();


/*
 * Retrieve token statement info from the hashmap using the 
 * keyword's value
 */
__das_WavefrontObjStatement *das_GetTokenInfo(char *id);

#endif