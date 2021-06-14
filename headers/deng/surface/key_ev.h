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


#ifndef __KEY_EV_H
#define __KEY_EV_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KEY_EV_C
    #include <stdlib.h> // malloc()
    #include <stdarg.h>   
    #include <math.h>

    // DEBUGGING PURPOSES
    #include <common/base_types.h>
    #include <common/common.h>
    #include <common/cerr_def.h>
    #include <vulkan/vulkan.h>
    #ifdef __linux__
        #include <X11/XKBlib.h>
    #endif

    #ifdef _WIN32
        #include <Windows.h>
    #endif

    #include <deng/surface/key_definitions.h>
    #include <deng/surface/surface_window.h>


    /// Key event registry arrays
    deng_bool_t active_ev[DENG_INPUT_EV_COUNT] = {0};
    deng_bool_t released_ev[DENG_INPUT_EV_COUNT] = {0};
#endif


/// Create new input bitmask from multiple input events
deng_InputBits deng_CreateInputMask(deng_ui32_t ev_c, ...);


/// Unmask deng_InputBits instance and return a static array of deng_InputEv, which size is exactly 8.
/// NOTE: Array elements that have no event attached use DENG_KEY_UNKNOWN as a value
deng_InputEv *deng_UnmaskInput(deng_InputBits bits);


/// Register new keyevent to key register
/// This function is meant to be called only by DENG platform dependant surface instances
void __deng_RegisterKeyEvent (
    deng_Key key, 
    deng_MouseButton btn, 
    deng_InputType in_type, 
    deng_InputEventType ev_type
);


/// Clean released key and mouse button array 
void __deng_UnreleaseKeys();


/// Find given key or mouse button status from specified event array
deng_bool_t __deng_FindKeyStatus (
    deng_Key key, 
    deng_MouseButton btn, 
    deng_InputType in_type, 
    deng_InputEventType ev_type
);

#ifdef __cplusplus
}
#endif

#endif

