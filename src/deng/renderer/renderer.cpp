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


#define __RENDERER_CPP
#include <deng/renderer/renderer.h>

namespace deng {

    Renderer::Renderer(deng_RendererHintBits hints, const dengMath::vec4<deng_vec_t> &env_color) :
        __DataUpdater(m_reg, m_api_bits, m_vk_rend, m_vk_vars) {
        m_hints = hints;
        m_env_color = env_color;
        m_api_bits = !(hints & DENG_RENDERER_HINT_API_VULKAN) ? hints & DENG_RENDERER_HINT_API_OPENGL :
                     hints & DENG_RENDERER_HINT_API_VULKAN;

        LOG("Final api bits: " + std::to_string(m_api_bits));

        // Throw error if no api backend was specified
        if(m_api_bits != DENG_RENDERER_HINT_API_VULKAN && m_api_bits != DENG_RENDERER_HINT_API_OPENGL)
            RUN_ERR("deng::Renderer::Renderer()", "Failed to initialise DENG renderer! No graphics api were specified.");
    }

    
    /// Setup graphics api specific renderer instance from the hints given
    void Renderer::setup(Camera3D &main_cam, Window &main_win) {
        m_p_cam = &main_cam;
        m_p_win = &main_win;

        // Check the current api bits
        if(m_api_bits == DENG_RENDERER_HINT_API_VULKAN) {
            // Setup Vulkan renderer configuration values
            m_vk_vars = std::make_shared<vulkan::__vk_ConfigVars>();
            m_vk_vars->background = m_env_color,
            m_vk_vars->p_cam = &main_cam;
            m_vk_vars->p_win = &main_win;
                
            m_vk_vars->cli_count_fps = (m_hints & DENG_RENDERER_HINT_SHOW_CMD_FPS_COUNTER) > 1 ? 1 : m_hints & DENG_RENDERER_HINT_SHOW_CMD_FPS_COUNTER,
            m_vk_vars->gui_count_fps = (m_hints & DENG_RENDERER_HINT_SHOW_GUI_FPS_COUNTER) > 1 ? 1 : m_hints & DENG_RENDERER_HINT_SHOW_GUI_FPS_COUNTER,
            m_vk_vars->enable_validation_layers = (m_hints & DENG_RENDERER_HINT_ENABLE_API_DEBUGGING) > 1 ? 1 : m_hints & DENG_RENDERER_HINT_ENABLE_API_DEBUGGING,

            // Create a new renderer instance
            m_vk_rend = std::make_shared<vulkan::__vk_Renderer>(*m_vk_vars.get(), Renderer::m_reg, m_assets, m_textures);

            // Tell Vulkan renderer to initialise Vulkan specific asset and texture instances 
            __AssetManager::submitTextureQueue();
            __AssetManager::submitAssetQueue(m_vk_vars->background);

            // Setup the renderer
            m_vk_rend->setup();
        }
        else RUN_ERR("deng::Renderer::setup()", "OpenGL is not supported in DENG :(");

        m_is_init = true;
    }


    /// Update data before creating new frame
    void Renderer::update() {
        m_p_win->update();
        m_p_cam->update();

        // Render new frame according to the renderer api used
        if(m_api_bits == DENG_RENDERER_HINT_API_VULKAN)
            m_vk_rend->makeFrame();
    }


    /// Begin the rendering loop
    void Renderer::run() {
        while(deng_IsRunning()) {
            m_p_win->update();
            m_p_cam->update();

            // Render new frame according to the renderer api used
            if(m_api_bits == DENG_RENDERER_HINT_API_VULKAN)
                m_vk_rend->makeFrame();

            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
        
        // Idle the renderer for destruction
        if(m_api_bits == DENG_RENDERER_HINT_API_VULKAN)
            m_vk_rend->idle();
    }


    /// idle the renderer 
    void Renderer::idle() {
        if(m_api_bits == DENG_RENDERER_HINT_API_VULKAN)
            m_vk_rend->idle();
    }
    

    void Renderer::setUIDataPtr(__ImGuiData *p_data) {
        // Check the backend renderer and call the correct ui data setter method accordingly
        if(m_api_bits == DENG_RENDERER_HINT_API_VULKAN)
            m_vk_rend->setUIDataPtr(p_data);
    }


    std::shared_ptr<vulkan::__vk_Renderer> Renderer::getVkRenderer() { return m_vk_rend; }
    __GlobalRegistry &Renderer::getRegistry() { return m_reg; }
    deng_RendererHintBits Renderer::getCurApiBackend() { return m_api_bits; }
} 
