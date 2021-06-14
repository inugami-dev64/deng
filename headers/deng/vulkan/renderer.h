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


#ifndef __VK_RENDERER_H
#define __VK_RENDERER_H

#ifdef __VK_RENDERER_CPP
    #include <vector>
    #include <memory>
    #include <chrono>
    #include <queue>
    #include <array>
    
    #include <vulkan/vulkan.h>
    #include <common/base_types.h>
    #include <common/hashmap.h>
    #include <common/uuid.h>
    #include <common/err_def.h>
    #include <data/assets.h>

    #include <math/deng_math.h>
    #include <deng/window.h>
    #include <deng/camera.h>

    #include <deng/lighting/light_srcs.h>
    #include <deng/registry/registry.h>
    #include <imgui-layer/imgui_entity.h>
#endif


/// Include all vulkan headers
#include <deng/vulkan/qm.h>
#include <deng/vulkan/sd.h>
#include <deng/vulkan/resources.h>
#include <deng/vulkan/rend_infos.h>
#include <deng/vulkan/ic.h>
#include <deng/vulkan/scc.h>
#include <deng/vulkan/pipeline_data.h>
#include <deng/vulkan/pipelines.h>
#include <deng/vulkan/desc_set_layout.h>
#include <deng/vulkan/desc_pool.h>
#include <deng/vulkan/desc_sets.h>
#include <deng/vulkan/dc.h>
#include <deng/vulkan/tm.h>
#include <deng/vulkan/ubm.h>
#include <deng/vulkan/bm.h>
#include <deng/vulkan/rm.h>

#include <deng/vulkan/runtime_updater.h>
#include <deng/vulkan/rend_init.h>


namespace deng {
    namespace vulkan {

        /// Main renderer class for Vulkan API
        class __vk_Renderer : private __vk_RendererInitialiser,
                              public __vk_RuntimeUpdater 
        {
        private:
            __vk_ConfigVars m_config;
            deng_bool_t m_is_init = false;
            deng_bool_t m_is_idle = true;

        private:

            /// Free and destroy all active Vulkan API specific instances
            void __cleanup();


            /// Additional cleanup methods
            void __cleanRendImgResources();
            void __cleanDrawCommands();
            void __cleanTextures();
            void __cleanAssets();
            void __cleanPipelines();
            void __cleanDescPools();
            void __cleanDescSetLayouts();
            void __freeBuffers();
            void __cleanSemaphores();
            void __cleanDevice();

            void __makeFrame();

        public:
            __vk_Renderer(__vk_ConfigVars &cnf, deng::__GlobalRegistry &reg,
                std::vector<deng_Id> &assets, std::vector<deng_Id> &textures);


            ~__vk_Renderer();

            /// Prepare assets for rendering
            void prepareAsset(deng_Id id);


            /// Prepare textures for binding them with assets
            void prepareTexture(deng_Id id);

            
            /// Update all currently available light sources
            void setLighting(std::array<deng_Id, __DENG_MAX_LIGHT_SRC_COUNT> &light_srcs);

            
            /// Submit new draw call
            void makeFrame();


            /// Setup the renderer, create descriptor sets, allocate buffers
            /// and record command buffers
            void setup();


            /// Wait for any queue operation to finish
            /// This method needs to be called whenever any 
            /// command or data buffers need to be updated
            void idle();

        // Setter and getter methods
        public:
            void setUIDataPtr(__ImGuiData *p_data);
            const deng_bool_t isInit();
            const __vk_BufferData &getBufferData();
        };
    }
}

#endif
