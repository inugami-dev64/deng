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


#define __COLLISION_CPP
#include <utils/collision.h>

namespace dengUtils {
    /*********************************************/
    /******* 2D COLLISION DETECTION METHODS ******/
    /*********************************************/

    /*
     * Find point collision with asset
     */
    deng_bool_t Collision2D::findPtCollision (
        dengMath::vec2<deng_vec_t> point,
        das_Asset *p_asset,
        dengMath::vec2<size_t> vert_bounds
    ) {
        size_t i, j;
        deng_bool_t is_colliding = false;
        dengMath::vec2<deng_vec_t> *vc;
        dengMath::vec2<deng_vec_t> *vn;

        // Check every triangle for potential collision
        switch (p_asset->asset_mode)
        {
        case DAS_ASSET_MODE_2D_UNMAPPED:
            for(i = vert_bounds.first; i < vert_bounds.second; i++) {
                j = i + 1;
                if(j == vert_bounds.second) 
                    j = vert_bounds.first;
                
                vc = (dengMath::vec2<deng_vec_t>*) &p_asset->vertices.uni_vert.vu2d[i].vert_data;
                vn = (dengMath::vec2<deng_vec_t>*) &p_asset->vertices.uni_vert.vu2d[j].vert_data;

                if
                (
                    ((vc->second >= point.second && vn->second < point.second) ||
                    (vc->second < point.second && vn->second >= point.second)) &&
                    (point.first < (vn->first - vc->first) * (point.second - vc->second) / (vn->second - vc->second) + vc->first)
                ) is_colliding = !is_colliding;
            }
            break;

        case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
            for(i = vert_bounds.first; i < vert_bounds.second; i++) {
                j = i + 1;
                if(j == vert_bounds.second) 
                    j = vert_bounds.first;

                vc = (dengMath::vec2<deng_vec_t>*) &p_asset->vertices.uni_vert.vm2d[i].vert_data;
                vn = (dengMath::vec2<deng_vec_t>*) &p_asset->vertices.uni_vert.vm2d[j].vert_data;

                if
                (
                    ((vc->second >= point.second && vn->second < point.second) ||
                    (vc->second < point.second && vn->second >= point.second)) &&
                    (point.first < (vn->first - vc->first) * (point.second - vc->second) / (vn->second - vc->second) + vc->first)
                ) is_colliding = !is_colliding;

            }
            break;

        default:
            break;
        }

        return is_colliding;
    }


    /*
     * Find point collision with specified vertices
     */
    deng_bool_t Collision2D::findPtCollision (
        dengMath::vec2<deng_vec_t> point,
        das_ObjVertData2D *verts,
        deng_ui32_t vert_c
    ) {
        size_t i, j;
        deng_bool_t is_colliding = false;
        dengMath::vec2<deng_vec_t> *vn, *vc;

        for(i = 0; i < vert_c; i++) {
            j = i + 1;
            if(j >= vert_c)
                j = 0;

            vc = (dengMath::vec2<deng_vec_t>*) &verts[i];
            vn = (dengMath::vec2<deng_vec_t>*) &verts[j];

            if
            (
                ((vc->second >= point.second && vn->second < point.second) ||
                (vc->second < point.second && vn->second >= point.second)) &&
                (point.first < (vn->first - vc->first) * (point.second - vc->second) / (vn->second - vc->second) + vc->first)
            ) is_colliding = !is_colliding;
        }

        return is_colliding;
    }
}
