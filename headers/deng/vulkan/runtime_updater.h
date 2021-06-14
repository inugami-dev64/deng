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
 *  Name: runtime_updater - Runtime buffer data updater
 *  Purpose: Provide a class interface for manipulating data during runtime
 *  Author: Karl-Mihkel Ott
 */ 


#ifndef __RUNTIME_UPDATER_H
#define __RUNTIME_UPDATER_H

#ifdef __RUNTIME_UPDATE_CPP
    #include <vector>
    #include <array>
    #include <mutex>
    #include <queue>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/err_def.h>
    #include <data/assets.h>
    #include <math/deng_math.h>

    #include <deng/window.h>
    #include <utils/timer.h>
    #include <deng/camera.h>

    #include <deng/vulkan/qm.h>
    #include <deng/vulkan/sd.h>
    #include <deng/vulkan/resources.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>

    #include <deng/vulkan/rend_infos.h>
    #include <deng/vulkan/ic.h>
    #include <deng/vulkan/scc.h>
    #include <deng/vulkan/desc_set_layout.h>
    #include <deng/vulkan/desc_pool.h>
    #include <deng/vulkan/desc_sets.h>
    #include <deng/vulkan/pipeline_data.h>
    #include <deng/vulkan/pipelines.h>
    #include <deng/vulkan/dc.h>
    #include <deng/vulkan/tm.h>
    #include <deng/vulkan/ubm.h>
    #include <deng/vulkan/bm.h>
    #include <deng/vulkan/rm.h>
#endif


namespace deng {
    namespace vulkan {

        class __vk_RuntimeUpdater {
        private:
            __vk_InstanceCreator &m_ic;
            __vk_SwapChainCreator &m_scc;
            __vk_DrawCaller &m_dc;
            __vk_ResourceManager &m_rm;
            __vk_DescriptorSetsCreator &m_desc_c;
            __GlobalRegistry &m_reg;
            std::vector<deng_Id> &m_assets;
            std::vector<deng_Id> &m_tex;

        protected:
            __ImGuiData *m_p_gui_data = NULL;

            // Flag which triggers commandbuffer level switching
            deng_bool_t m_switch_flag = false;

        public:
            __vk_RuntimeUpdater (
                __vk_InstanceCreator &ic,
                __vk_SwapChainCreator &scc,
                __vk_DrawCaller &dc,
                __vk_ResourceManager &rm,
                __vk_DescriptorSetsCreator &desc_c,
                __GlobalRegistry &reg,
                std::vector<deng_Id> &assets, 
                std::vector<deng_Id> &tex
            );

            /// This method updates the vertices buffer that is allocated by given assets
            void updateAssetData(const dengMath::vec2<deng_ui32_t> &bounds);


            /// Update ImGui vertices and indices
            void updateUIData(const dengMath::vec4<deng_vec_t> &background);


            /// Rerecord existing commandbuffers 
            void updateCmdBuffers(const dengMath::vec4<deng_vec_t> &background);

            
            /// Reallocate or allocate new descriptor sets for assets between given bounds
            /// NOTE: Vulkan renderer must be idled
            void updateAssetDS(const dengMath::vec2<deng_ui32_t> &bounds);


            /// Reallocate main buffer and copy all asset data to it if needed
            /// NOTE: Vulkan renderer must be idled
            void checkForReallocation();


            /// Update texture texel data, however it does not perform any texture size adjustments. For that
            /// consider creating a new texture
            void updateTexelData(__vk_Texture &tex);
        };
    }
}

#endif
