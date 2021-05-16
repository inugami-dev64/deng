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


#define __CAMERA_MAT_CPP
#include <math/camera_mat.h>


namespace dengMath {
    
    CameraMatrix::CameraMatrix(deng_CameraType cam_type) {
        // Set the default camera positions and rotations according to the camera type specified

        // I will probably add this to the constructor of mat4
        m_rot_x_mat.row1 = vec4<deng_vec_t>{1.0f, 0.0f, 0.0f, 0.0f}; 
        m_rot_x_mat.row2 = vec4<deng_vec_t>{0.0f, 1.0f, 0.0f, 0.0f}; 
        m_rot_x_mat.row3 = vec4<deng_vec_t>{0.0f, 0.0f, 1.0f, 0.0f}; 
        m_rot_x_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f}; 

        m_rot_y_mat.row1 = vec4<deng_vec_t>{1.0f, 0.0f, 0.0f, 0.0f}; 
        m_rot_y_mat.row2 = vec4<deng_vec_t>{0.0f, 1.0f, 0.0f, 0.0f}; 
        m_rot_y_mat.row3 = vec4<deng_vec_t>{0.0f, 0.0f, 1.0f, 0.0f}; 
        m_rot_y_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f}; 

        switch(cam_type)
        {
        case DENG_CAMERA_TYPE_FPP:
            m_camera_pos = vec4<deng_vec_t>{
                DENG_FPP_CAMERA_DEFAULT_POS_X,
                DENG_FPP_CAMERA_DEFAULT_POS_Y,
                DENG_FPP_CAMERA_DEFAULT_POS_Z,
                1.0f,
            };

            m_x_rot = 0.0f;
            m_y_rot = 0.0f;

            camTransform(false);
            break;

        case DENG_CAMERA_TYPE_EDITOR:
            m_camera_pos = vec4<deng_vec_t>{
                DENG_EDITOR_CAMERA_DEFAULT_POS_X,
                DENG_EDITOR_CAMERA_DEFAULT_POS_Y,
                DENG_EDITOR_CAMERA_DEFAULT_POS_Z,
                1.0f
            };

            m_x_rot = 0;
            m_y_rot = 0;
            camTransform(true);
            break;
        }
        
        //m_camera_mat.row1 = vec4<deng_vec_t>{1.0f, 0.0f, 0.0f, 0.0f};
        //m_camera_mat.row2 = vec4<deng_vec_t>{0.0f, 1.0f, 0.0f, 0.0f};
        //m_camera_mat.row3 = vec4<deng_vec_t>{0.0f, 0.0f, 1.0f, 0.0f};
        //m_camera_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};
    }


    /* 
     * Force set camera position to a new one
     */
    void CameraMatrix::setCameraPosition(const vec3<deng_vec_t> &camera_pos) {
        m_camera_pos.first = camera_pos.first;
        m_camera_pos.second = camera_pos.second;
        m_camera_pos.third = camera_pos.third;
        m_camera_pos.fourth = 1.0f;
    }


    /*
     * Move camera by one movement step
     */
    void CameraMatrix::moveCamera (
        const vec3<deng_vec_t> &mov_speed, 
        deng_bool_t is_world,
        deng_bool_t ignore_pitch,
        const deng_CoordinateAxisType &movement_type
    ) {
        vec4<deng_vec_t> mov = {0.0f, 0.0f, 0.0f, 0.0f};

        switch(movement_type)
        {
        case DENG_COORD_AXIS_X:
            mov.first = mov_speed.first;
            if(ignore_pitch && !is_world)
                mov = m_rot_y_mat * mov;
            else if(!is_world) mov = m_rot_y_mat * m_rot_x_mat * mov;
            break;

        case DENG_COORD_AXIS_Y:
            mov.second = mov_speed.second;
            if(ignore_pitch && !is_world)
                mov = m_rot_y_mat * mov;
            else if(!is_world) mov = m_rot_y_mat * m_rot_x_mat * mov;
            break;

        case DENG_COORD_AXIS_Z:
            mov.third = mov_speed.third;
            if(ignore_pitch && !is_world)
                mov = m_rot_y_mat * mov;
            else if(!is_world) mov = m_rot_y_mat * m_rot_x_mat * mov;
            break;

        case DENG_COORD_AXIS_UNDEFINED:
            mov.first = mov_speed.first;
            mov.second = mov_speed.second;
            mov.third = mov_speed.third;

            if(ignore_pitch && !is_world)
                mov = m_rot_y_mat * mov;
            else if(!is_world) mov = m_rot_y_mat * m_rot_x_mat * mov;
            break;
        
        default:
            break;
        }

        m_camera_pos += mov;
    }


    /// Set new rotation for the camera relative to the its coordinate system
    void CameraMatrix::setCameraRotation (
        deng_vec_t x_rot, 
        deng_vec_t y_rot
    ) {
        m_x_rot = x_rot;
        m_y_rot = y_rot;

        m_rot_x_mat.row1 = vec4<deng_vec_t>{1.0f, 0.0f, 0.0f, 0.0f};
        m_rot_x_mat.row2 = vec4<deng_vec_t>{0.0f, cosf(m_x_rot), -sinf(m_x_rot), 0.0f};
        m_rot_x_mat.row3 = vec4<deng_vec_t>{0.0f, sinf(m_x_rot), cosf(m_x_rot), 0.0f};
        m_rot_x_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};
        
        m_rot_y_mat.row1 = vec4<deng_vec_t>{cosf(m_y_rot), 0.0f, -sinf(m_y_rot), 0.0f};
        m_rot_y_mat.row2 = vec4<deng_vec_t>{0.0f, 1.0f, 0.0f, 0.0f};
        m_rot_y_mat.row3 = vec4<deng_vec_t>{sinf(m_y_rot), 0.0f, cosf(m_y_rot), 0.0f};
        m_rot_y_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};
    }


    /*
     * Set new rotation for the camera relative to its origin point in world coordinates
     */
    void CameraMatrix::setOriginRotation (
        dengMath::vec3<deng_vec_t> point,
        deng_vec_t x_rot,
        deng_vec_t y_rot
    ) {
        m_x_rot = x_rot;
        m_y_rot = y_rot;

        mat4<deng_vec_t> transform_mat;
        transform_mat.row1 = vec4<deng_vec_t>{1.0f, 0.0f, 0.0f, m_camera_pos.first - point.first};
        transform_mat.row2 = vec4<deng_vec_t>{0.0f, 1.0f, 0.0f, m_camera_pos.second - point.second};
        transform_mat.row3 = vec4<deng_vec_t>{0.0f, 0.0f, 1.0f, m_camera_pos.third - point.third};
        transform_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};

        mat4<deng_vec_t> tmp_rot_mat;
        tmp_rot_mat.row1 = vec4<deng_vec_t>{1.0f, 0.0f, 0.0f, 0.0f};
        tmp_rot_mat.row2 = vec4<deng_vec_t>{0.0f, (deng_vec_t) cosf(m_x_rot), (deng_vec_t) -sinf(m_x_rot), 0.0f};
        tmp_rot_mat.row3 = vec4<deng_vec_t>{0.0f, (deng_vec_t) sinf(m_x_rot), (deng_vec_t) cosf(m_x_rot), 0.0f};
        tmp_rot_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};

        m_rot_x_mat = transform_mat * tmp_rot_mat * transform_mat.inv();

        tmp_rot_mat.row1 = vec4<deng_vec_t>{(deng_vec_t) cosf(m_y_rot), 0.0f, (deng_vec_t) sinf(m_y_rot), 0.0f},
        tmp_rot_mat.row2 = vec4<deng_vec_t>{0.0f, 1.0f, 0.0f, 0.0f},
        tmp_rot_mat.row3 = vec4<deng_vec_t>{(deng_vec_t) -sinf(m_y_rot), 0.0f, (deng_vec_t) cosf(m_y_rot), 0.0f},
        tmp_rot_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};

        m_rot_y_mat = transform_mat * tmp_rot_mat * transform_mat.inv();
    }


    /*
     * Create transformation matrix for camera system based
     * on previously submitted values
     */
    void CameraMatrix::camTransform(deng_bool_t is_local_coord_sys) {
        if(!is_local_coord_sys) {
            m_rs = m_rot_y_mat * m_rot_x_mat * __DENG_CAMERA_RIGHT;
            m_fs = m_rot_y_mat * m_rot_x_mat * __DENG_CAMERA_FORWARD;
            m_ts = m_rot_y_mat * m_rot_x_mat * __DENG_CAMERA_UP;

            //mat4<deng_vec_t> basis;
            vec3<deng_vec_t> translation;
            translation.first = -(m_rs * m_camera_pos);
            translation.second = -(m_ts * m_camera_pos);
            translation.third = -(m_fs * m_camera_pos);

            m_camera_mat.row1 = vec4<deng_vec_t>{m_rs.first, m_rs.second, m_rs.third, translation.first};
            m_camera_mat.row2 = vec4<deng_vec_t>{m_ts.first, m_ts.second, m_ts.third, translation.second};
            m_camera_mat.row3 = vec4<deng_vec_t>{m_fs.first, m_fs.second, m_fs.third, translation.third};
            m_camera_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};
        }

        else {
            m_rs = m_rot_y_mat * m_rot_x_mat * __DENG_CAMERA_RIGHT;
            m_ts = m_rot_y_mat * m_rot_x_mat * __DENG_CAMERA_UP;
            m_fs = m_rot_y_mat * m_rot_x_mat * __DENG_CAMERA_FORWARD;
            vec4<deng_vec_t> translation = m_rot_y_mat * m_rot_x_mat * m_camera_pos;

            m_camera_mat.row1 = vec4<deng_vec_t>{m_rs.first, m_rs.second, m_rs.third, translation.first};
            m_camera_mat.row2 = vec4<deng_vec_t>{m_ts.first, m_ts.second, m_ts.third, translation.second};
            m_camera_mat.row3 = vec4<deng_vec_t>{m_fs.first, m_fs.second, m_fs.third, translation.third};
            m_camera_mat.row4 = vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f};
        }
    }


    /*
     * Get the transformation matrix
     */
    mat4<deng_vec_t> CameraMatrix::getTransformMat() {
        // Transpose is necessary since the matrix memory layout is different on shaders
        return m_camera_mat.transpose();
    }


    /*
     * Get the current position of the camera
     */
    vec4<deng_vec_t> CameraMatrix::getPosition() {
        return m_camera_pos;
    }


    /*
     * Get current camera sides' coordinates
     */
    void CameraMatrix::getSides (
        vec4<deng_vec_t> *p_u,
        vec4<deng_vec_t> *p_v,
        vec4<deng_vec_t> *p_w
    ) {
        if(p_u)
            *p_u = m_rs;
        if(p_v)
            *p_v = m_ts;
        if(p_w)
            *p_w = m_fs;
    }
}