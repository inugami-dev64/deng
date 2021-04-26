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


#define __MESH_GEN_CPP
#include <utils/mesh_gen.h>

namespace dengUtils {
    /****************************************/
    /****************************************/
    /********** CubeGenerator class *********/
    /****************************************/
    /****************************************/

    CubeGenerator::CubeGenerator() {
        // Base cube has total dimentions of 1.0 x 1.0 x 1.0
        m_base_cube_verts[0] = dengMath::vec4<deng_vec_t>{-0.5f, -0.5f, -0.5f, 1.0f};
        m_base_cube_verts[1] = dengMath::vec4<deng_vec_t>{0.5f, -0.5f, -0.5f, 1.0f};
        m_base_cube_verts[2] = dengMath::vec4<deng_vec_t>{0.5f, 0.5f, -0.5f, 1.0f};
        m_base_cube_verts[3] = dengMath::vec4<deng_vec_t>{-0.5f, 0.5f, -0.5f, 1.0f};

        m_base_cube_verts[4] = dengMath::vec4<deng_vec_t>{-0.5f, -0.5f, 0.5f, 1.0f};
        m_base_cube_verts[5] = dengMath::vec4<deng_vec_t>{0.5f, -0.5f, 0.5f, 1.0f};
        m_base_cube_verts[6] = dengMath::vec4<deng_vec_t>{0.5f, 0.5f, 0.5f, 1.0f};
        m_base_cube_verts[7] = dengMath::vec4<deng_vec_t>{-0.5f, 0.5f, 0.5f, 1.0f};


        // Specify the flat hexahedron texture mapping
        m_base_cube_tex_verts[0] = {-0.33f, -1.0f};
        m_base_cube_tex_verts[1] = {0.33f, -1.0f};

        m_base_cube_tex_verts[2] = {-1.0f, -0.5f};
        m_base_cube_tex_verts[3] = {-0.33f, -0.5f};
        m_base_cube_tex_verts[4] = {0.33f, -0.5f};
        m_base_cube_tex_verts[5] = {1.0f, -0.5f};

        m_base_cube_tex_verts[6] = {-1.0f, 0.0f};
        m_base_cube_tex_verts[7] = {-0.33f, 0.0f};
        m_base_cube_tex_verts[8] = {0.33f, 0.0f};
        m_base_cube_tex_verts[9] = {1.0f, 0.0f};

        m_base_cube_tex_verts[10] = {-0.33f, 0.5f};
        m_base_cube_tex_verts[11] = {0.33f, 0.5f};

        m_base_cube_tex_verts[11] = {-0.33f, 1.0f};
        m_base_cube_tex_verts[12] = {0.33f, 1.0f};

        m_base_cube_inds = {{
            {5, 1}, {4, 0}, {7, 3}, {7, 3}, {6, 4}, {5, 1}, // back face
            {4, 2}, {0, 6}, {3, 7}, {3, 7}, {7, 3}, {4, 2}, // left face
            {0, 10}, {1, 11}, {2, 8}, {2, 8}, {3, 7}, {0, 10}, // front face
            {1, 9}, {5, 5}, {6, 4}, {6, 4}, {2, 8}, {1, 9}, // right face
            {4, 12}, {5, 13}, {1, 11}, {1, 11}, {0, 10}, {4, 12}, // top face
            {3, 7}, {2, 8}, {6, 4}, {6, 4}, {7, 3}, {3, 7} // bottom face
        }};
    }


    /*
     * Sort cube vertices and indices into correct data types
     * for texture mapped cube
     */
    void CubeGenerator::__sortTexMapped (
        std::vector<VERT_MAPPED> &vert,
        std::vector<deng_ui32_t> &ind
    ) {
        Hashmap hm = {0};
        newHashmap(&hm, 4 * __CUBE_INDICES_C);

        size_t unique_c = 0;
        size_t old_vert_c = vert.size();
        size_t old_ind_c = ind.size();
        vert.reserve(old_vert_c + __CUBE_INDICES_C);
        ind.resize(old_ind_c + __CUBE_INDICES_C);

        deng_ui32_t *p_val = NULL;
        
        for(size_t i = 0; i < m_base_cube_inds.size(); i++) {
            VERT_MAPPED key;
            key.vert_data.vert_x = m_base_cube_verts[m_base_cube_inds[i].first].first;
            key.vert_data.vert_y = m_base_cube_verts[m_base_cube_inds[i].first].second;
            key.vert_data.vert_z = m_base_cube_verts[m_base_cube_inds[i].first].third;
            key.tex_data = m_base_cube_tex_verts[m_base_cube_inds[i].second];

            p_val = (deng_ui32_t*) findValue ( 
                &hm, 
                &key,
                sizeof(VERT_MAPPED)
            );

            // No value found, increment index and add value to hashmap
            if(!p_val) {
                vert.resize(unique_c + 1);
                vert[unique_c] = key;
                ind[i + old_ind_c] = static_cast<deng_ui32_t>(unique_c);
                unique_c++;

                pushToHashmap (
                    &hm,
                    &vert[vert.size() - 1],
                    sizeof(VERT_MAPPED),
                    &ind[i]
                );
            }

            else ind[i + old_ind_c] = *p_val;
        }
    }


    /*
     * Construct transformation matrix based on given parameters
     */
    dengMath::mat4<deng_vec_t> CubeGenerator::__mkTransformMatrix (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin
    ) {
        dengMath::vec4<deng_vec_t> rl_pos = {pos.first, pos.second, pos.third, 1.0f};
        dengMath::mat4<deng_vec_t> tf_mat;

        // Set scaling of the cube
        tf_mat.row1.first = size.first;
        tf_mat.row2.second = size.second;
        tf_mat.row3.third = size.third;
        tf_mat.row4.fourth = 1.0f;

        // Calculate position that has considered origin
        rl_pos = tf_mat * rl_pos;
        rl_pos += {origin.first, origin.second, origin.third, 1.0f};

        // Set transformation
        tf_mat.row1.fourth = rl_pos.first;
        tf_mat.row2.fourth = rl_pos.second;
        tf_mat.row3.fourth = rl_pos.third;
        tf_mat.row4.fourth = rl_pos.fourth;

        return tf_mat;
    }


    /*
     * Generate unmapped cube with position, size, origin and color
     * parameters
     */
    void CubeGenerator::generateUnmappedCube (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin,
        std::vector<VERT_UNMAPPED> &vert,
        std::vector<deng_ui32_t> &ind
    ) {
        dengMath::mat4<deng_vec_t> tf_mat = __mkTransformMatrix (
            pos,
            size,
            origin
        );

        size_t old_vert_c = vert.size();
        for(size_t i = old_vert_c; i < vert.size(); i++) {
            dengMath::vec4<deng_vec_t> tmp = tf_mat * m_base_cube_verts[i - old_vert_c];
            vert[i].vert_data.vert_x = tmp.first;
            vert[i].vert_data.vert_y = tmp.second;
            vert[i].vert_data.vert_z = tmp.third;
        
            ind[i] = m_base_cube_inds[i - old_vert_c].first;
        }
    }


    /*
     * Generate texture mapped cube with position, size and origin parameters
     */
    void CubeGenerator::generateMappedCube (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin,
        std::vector<VERT_MAPPED> &vert,
        std::vector<deng_ui32_t> &ind
    ) {
        dengMath::mat4<deng_vec_t> tf_mat = __mkTransformMatrix (
            pos,
            size,
            origin
        );

        size_t old_vert_c = vert.size();
        __sortTexMapped(vert, ind);

        // Apply transformations
        for(size_t i = old_vert_c; i < vert.size(); i++) {
            dengMath::vec4<deng_vec_t> tmp{};
            tmp = vert[i].vert_data;
            tmp = tf_mat * tmp;

            vert[i].vert_data.vert_x = tmp.first;
            vert[i].vert_data.vert_y = tmp.second;
            vert[i].vert_data.vert_z = tmp.third;
        }
    }


    /*
     * Generate unmapped cube asset 
     */
    das_Asset CubeGenerator::generateUnmappedCubeAsset (
        const dengMath::vec3<deng_vec_t> &pos, 
        const dengMath::vec3<deng_vec_t> &size, 
        const dengMath::vec3<deng_vec_t> &origin, 
        char *name
    ) {
        std::vector<VERT_UNMAPPED> vert;
        std::vector<deng_ui32_t> ind;

        generateUnmappedCube (
            pos, 
            size, 
            origin, 
            vert, 
            ind
        );

        // Create a new asset and copy all data over
        das_Asset asset;
        asset.asset_mode = DAS_ASSET_MODE_3D_UNMAPPED;
        asset.uuid = uuid_Generate();
        asset.name = name;
        asset.vertices.n = vert.size();
        asset.indices.n = ind.size();

        asset.vertices.uni_vert.vun = (VERT_UNMAPPED*) malloc(asset.vertices.n * sizeof(VERT_UNMAPPED));
        asset.indices.indices = (deng_ui32_t*) malloc(asset.indices.n * sizeof(deng_ui32_t));

        memcpy (
            asset.vertices.uni_vert.vuu,
            vert.data(),
            asset.vertices.n * sizeof(VERT_UNMAPPED)
        );

        memcpy (
            asset.indices.indices,
            ind.data(),
            asset.indices.n * sizeof(deng_ui32_t)
        );

        return asset;
    }


    /*
     * Generate texture mapped cube asset
     */
    das_Asset CubeGenerator::generateMappedCubeAsset (
        const dengMath::vec3<deng_vec_t> &pos,
        const dengMath::vec3<deng_vec_t> &size,
        const dengMath::vec3<deng_vec_t> &origin,
        char *name,
        char *tex_uuid
    ) {
        std::vector<VERT_MAPPED> vert;
        std::vector<deng_ui32_t> ind;
        
        generateMappedCube (
            pos,
            size, origin,
            vert,
            ind
        );

        das_Asset asset{};
        asset.asset_mode = DAS_ASSET_MODE_3D_TEXTURE_MAPPED;
        asset.uuid = uuid_Generate();
        asset.vk_id = NULL;
        asset.gl_id = NULL;
        asset.color = {1.0f, 1.0f, 1.0f, 1.0f};
        asset.force_unmap = true;
        asset.is_shown = true;
        asset.vertices.uni_vert.vmn = (VERT_MAPPED*) calloc (
            vert.size(),
            sizeof(VERT_MAPPED)
        );

        memcpy (
            asset.vertices.uni_vert.vmn,
            vert.data(),
            vert.size() * sizeof(VERT_MAPPED)
        );

        asset.vertices.n = vert.size();

        asset.indices.indices = (deng_ui32_t*) calloc (
            ind.size(),
            sizeof(deng_ui32_t)
        );

        memcpy (
            asset.indices.indices,
            ind.data(),
            ind.size() * sizeof(deng_ui32_t)
        );

        asset.indices.n = ind.size();
        asset.tex_uuid = NULL;
        
        return asset;
    }
}
