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


#version 450
#extension GL_ARB_separate_shader_objects : enable

const uint orthographic_camera_mode3D = 0x00000001u;
const uint perspective_camera_mode3D = 0x00000002u;

layout(binding = 0) uniform UniformBufferObj {
    mat4 transform;
    mat4 view;
    uint ubo_flag_bits;
} ubo;


/*
 * Store color information about asset when it is not texture mapped
 */
layout(binding = 1) uniform ColorData {
    vec4 color;
    int is_unmapped;
} cl;


layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm_pos;

layout(location = 0) out vec4 out_color;

void main() {
    if((ubo.ubo_flag_bits & orthographic_camera_mode3D) == orthographic_camera_mode3D)
        gl_Position = ubo.view * vec4(-in_pos[0], -in_pos[1], in_pos[2], 1.0f);
    
    else if((ubo.ubo_flag_bits & perspective_camera_mode3D) == perspective_camera_mode3D)
        gl_Position = ubo.transform * vec4(-in_pos[0], -in_pos[1], in_pos[2], 1.0f);

    else gl_Position = vec4(-in_pos[0], -in_pos[1], in_pos[2], 1.0f);

    out_color = cl.color;   
}
