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


//// This header contains all DAS error checking functions
#ifndef __DAS_CHECK_H
#define __DAS_CHECK_H


#ifdef __DAS_CHECK_C
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #include <common/base_types.h>
    #include <common/cerr_def.h>

    #include <data/das_file.h>
#endif


/*
 * Enumeral of all possible DAS format error codes
 */
typedef enum das_Error {
    DAS_ERROR_SUCCESS                           = 0,
    DAS_ERROR_FILE_CORRUPTION                   = 1,
    DAS_ERROR_INVALID_MAGIC_NUMBER              = 2,
    DAS_ERROR_SIZE_OUT_OF_BOUNDS                = 3,
    DAS_ERROR_INVALID_UUID                      = 4,
    DAS_ERROR_INVALID_REQUESTED_VERTEX_FORMAT   = 5,
    DAS_ERROR_INVALID_VERTICES_COUNT            = 6,
    DAS_ERROR_INVALID_INDICES_COUNT             = 7,
    DAS_ERROR_INVALID_FRAME_COUNT               = 8,
    DAS_ERROR_INVALID_ANIMATION_FRAME_COUNT     = 9
} das_Error;


// Read error definition macros
#define __DAS_READ_ERROR(file)                  fprintf(stderr, "Failed to read from file %s, possible file corruption\n", file), \
                                                exit(DAS_ERROR_FILE_CORRUPTION)

#define __DAS_READ_INVALID_VERT_FORMAT(file)    fprintf(stderr, "Invalid requested asset vertices mode in file: %s\n", file), \
                                                exit(DAS_ERROR_FILE_CORRUPTION)

#define __DAS_READ_INVALID_MAGIC_NUM(file)      fprintf(stderr, "Invalid magic number for DAS format in file %s\n", file), \
                                                exit(DAS_ERROR_INVALID_MAGIC_NUMBER)

#define __DAS_READ_CORRUPT_ERROR(file)          fprintf(stderr, "Corrupt DENG asset file: %s\n", file), \
                                                exit(DAS_ERROR_FILE_CORRUPTION)

// Write error definition macros


// Vertices and indices count check macros
#define das_VertCountCheck(vc) ( vc <= 0 ? DAS_ERROR_INVALID_VERTICES_COUNT : DAS_ERROR_SUCCESS )
#define das_IndicesCountCheck(ic) ( ic <= 0 ? DAS_ERROR_INVALID_INDICES_COUNT : DAS_ERROR_SUCCESS )
#define das_FrameCountCheck(fc) ( fc <= 0 ? DAS_ERROR_INVALID_ANIMATION_FRAME_COUNT : DAS_ERROR_SUCCESS )


/// Check if the requested memory size is available for reading otherwise
/// throw a runtime error
void das_ErrBufferReadCheck(size_t req_size, size_t max_size, const char *file_name);


/// Check if the given header specifier is valid
void das_CheckHdrName(char *name, char *exp_name, const char *file_name);


/// Check if provided uuid is valid
das_Error das_UuidCheck(char *uuid);


/// Check if magic number provided is valid or not
das_Error das_MagicNumberCheck(deng_ui64_t num);

#endif
