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
 * (d) If the Work includes a "NOTICE" m_texturest file as part of its
 *     distribution, then any Derivative Works that You distribute must
 *     include a readable copy of the attribution notices contained
 *     within such NOTICE file, excluding those notices that do not
 *     pertain to any part of the Derivative Works, in at least one
 *     of the following places: within a NOTICE m_texturest file distributed
 *     as part of the Derivative Works; within the Source form or
 *     documentation, if provided along with the Derivative Works; or,
 *     within a display generated by the Derivative Works, if and
 *     wherever such third-party notices normally appear. The contents
 *     of the NOTICE file are for informational purposes only and
 *     do not modify the License. You may add Your own attribution
 *     notices within Derivative Works that You distribute, alongside
 *     or as an addendum to the NOTICE m_texturest from the Work, provided
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


#define __VK_REND_INIT_CPP
#include <deng/vulkan/rend_init.h>


namespace deng {
    namespace vulkan {
        
        __vk_RendererInitialiser::__vk_RendererInitialiser (
            deng::Window &win, 
            const __vk_ConfigVars &conf,
            deng::__GlobalRegistry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures
        ) : m_reg(reg), m_assets(assets), m_textures(textures) {

            // Create new VkInstace
            m_ic = std::make_unique<__vk_InstanceCreator>(win, conf.enable_validation_layers);
            
            // Create new swapchain creator
            m_scc = std::make_unique<__vk_SwapChainCreator>(m_ic->getDev(),
                win, m_ic->getGpu(), m_ic->getSu(), m_ic->getQFF(),
                conf.msaa_sample_count);

            // Create new vulkan descriptor creator
            m_desc_c = std::make_unique<__vk_DescriptorSetsCreator>(m_ic->getDev(),
                m_scc->getExt(), m_scc->getRp(), m_reg, m_assets,
                m_textures, conf.msaa_sample_count);

            // Create new draw caller instance and make command pool
            m_dc = std::make_unique<__vk_DrawCaller>(m_ic->getDev(),
                m_ic->getQFF(), m_assets, m_textures, m_desc_c->getUIDS(), m_reg);
            
            m_dc->mkCommandPool(m_ic->getDev());

            __max_frame_c = static_cast<deng_ui32_t>(m_scc->getSCImg().size());

            // Create new buffer resources allocator
            m_rm = std::make_unique<__vk_ResourceManager>(m_ic->getDev(), 
                m_ic->getGpu(), m_scc->getExt(), conf.msaa_sample_count,
                m_scc->getRp(), m_dc->getComPool(), m_ic->getQFF().graphics_queue,
                m_scc->getSCImgViews(), m_reg, m_assets, m_textures, m_scc->getSF(),
                m_ic->getGpuLimits()); 

            // Create new vulkan pipeline creator
            m_pl_c = std::make_unique<__vk_PipelineCreator>(
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_UNMAPPED_2D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_UNMAPPED_3D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D),
                m_desc_c->getLayout(DENG_PIPELINE_TYPE_UI)
            );
        }


        /// Renderer initialiser getter methods
        __vk_InstanceCreator &__vk_RendererInitialiser::getIC() { return *m_ic.get(); }
        __vk_SwapChainCreator &__vk_RendererInitialiser::getSCC() { return *m_scc.get(); } 
        __vk_DescriptorSetsCreator &__vk_RendererInitialiser::getDescC() { return *m_desc_c.get(); }
        __vk_PipelineCreator &__vk_RendererInitialiser::getPipelineC() { return *m_pl_c.get(); }
        __vk_ResourceManager &__vk_RendererInitialiser::getResMan() { return *m_rm.get(); }
        __vk_DrawCaller &__vk_RendererInitialiser::getDrawCaller() { return *m_dc.get(); }


        void __vk_RendererInitialiser::setUIDataPtr(__ImGuiData *p_gui) { 
            m_rm->setUIDataPtr(p_gui); 
            m_dc->setUIDataPtr(p_gui);
        }
    }
}
