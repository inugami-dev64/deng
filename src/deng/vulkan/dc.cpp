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


#define __DC_CPP
#include <deng/vulkan/dc.h>

namespace deng {
    namespace vulkan {
        extern deng_ui32_t __max_frame_c;

        /*************************************/
        /*************************************/
        /********** __vk_DrawCaller **********/
        /*************************************/
        /*************************************/

        __vk_DrawCaller::__vk_DrawCaller (
            VkDevice device,
            __vk_QueueManager qff,
            const std::vector<deng_Id> &assets,
            const std::vector<deng_Id> &textures,
            const std::vector<VkDescriptorSet> &ui_sets,
            deng::__GlobalRegistry &reg
        ) : m_assets(assets), m_textures(textures), m_ui_sets(ui_sets), m_reg(reg) 
        {
            m_qff = qff;
            __mkSynchronisation(device);
        }


        /// Create command pool
        void __vk_DrawCaller::mkCommandPool(VkDevice device) {
            VkCommandPoolCreateInfo cmd_pool_createinfo{};
            cmd_pool_createinfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            cmd_pool_createinfo.queueFamilyIndex = m_qff.getGraphicsQFIndex();
            cmd_pool_createinfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            // Create cmd_pool
            if(vkCreateCommandPool(device, &cmd_pool_createinfo, NULL, &m_cmd_pool) != VK_SUCCESS)
                VK_DRAWCMD_ERR("failed to create command pool");
        }


        /// Create semaphores and fences for synchronising frames 
        void __vk_DrawCaller::__mkSynchronisation(VkDevice &device) {
            // Resize semaphores 
            image_available_semaphore_set.resize(__max_frame_c);
            render_finished_semaphore_set.resize(__max_frame_c);
            flight_fences.resize(__max_frame_c);
            VkSemaphoreCreateInfo semaphore_info{};
            semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fence_createInfo{};
            fence_createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for(size_t i = 0; i < __max_frame_c; i++) {
                if(vkCreateSemaphore(device, &semaphore_info, NULL, 
                   &image_available_semaphore_set[i]) != VK_SUCCESS ||
                   vkCreateSemaphore(device, &semaphore_info, NULL, 
                   &render_finished_semaphore_set[i]) != VK_SUCCESS ||
                   vkCreateFence(device, &fence_createInfo, NULL, 
                   &flight_fences[i]) != VK_SUCCESS) 
                    VK_DRAWCMD_ERR("Failed to create semaphores");
                else LOG("Successfully created semaphores");
            }
        }

        
        /// Bind asset resources to command buffers
        void __vk_DrawCaller::__bindAssetResources (
            das_Asset &asset, 
            VkCommandBuffer cur_buf,
            const __vk_BufferData &bd
        ) {
            // Vertex normal binding number variable, since it can change according to the asset mode
            deng_ui32_t nor_bind_nr = 1;

            // Bind the position vertex location in buffer
            vkCmdBindVertexBuffers(cur_buf, 0, 1, &bd.main_buffer, 
                &asset.offsets.pos_offset);


            // Check if texture vertices should be bound
            if(asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED) {
                // Bind texture vertex location in buffer
                vkCmdBindVertexBuffers(cur_buf, 1, 1, &bd.main_buffer, 
                    &asset.offsets.tex_offset);

                nor_bind_nr = 2;
            }

            // Check if vertex normals should be bound
            if(asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED ||
               asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) {
                // Bind vertex normals location in buffer
                vkCmdBindVertexBuffers(cur_buf, nor_bind_nr, 1, &bd.main_buffer,
                    &asset.offsets.nor_offset);
            }

            // Bind the position indices location in buffer
            vkCmdBindIndexBuffer(cur_buf, bd.main_buffer, 
                asset.offsets.ind_offset, VK_INDEX_TYPE_UINT32);
        }


        /// Bind ImGui resources to command buffers
        void __vk_DrawCaller::__bindUIElementResources(__ImGuiEntity *ent, VkCommandBuffer cur_buf, const __vk_BufferData &bd) {
            vkCmdBindVertexBuffers(cur_buf, 0, 1, &bd.main_buffer, &bd.asset_cap);
            LOG("Index offset: " + std::to_string(ent->buf_offset));
            vkCmdBindIndexBuffer(cur_buf, bd.main_buffer, bd.asset_cap + ent->buf_offset, 
                VK_INDEX_TYPE_UINT32);
        }


        /// Bind asset pipeline and return its pipeline layout
        VkPipelineLayout *__vk_DrawCaller::__bindAssetPipeline(das_Asset &asset, VkCommandBuffer cmd_buf) {
            VkPipeline *p_pl = NULL;
            VkPipelineLayout *p_pl_layout = NULL;
            // Find correct pipelines and their layouts according to the asset mode
            switch(asset.asset_mode) {
            case DAS_ASSET_MODE_2D_UNMAPPED:
                p_pl = &m_pl_data[UM2D_I].pipeline;
                p_pl_layout = m_pl_data[UM2D_I].p_pipeline_layout;
                break;

            case DAS_ASSET_MODE_2D_TEXTURE_MAPPED:
                p_pl = &m_pl_data[TM2D_I].pipeline;
                p_pl_layout = m_pl_data[TM2D_I].p_pipeline_layout;
                break;

            case DAS_ASSET_MODE_3D_UNMAPPED:
                p_pl = &m_pl_data[UM3D_I].pipeline;
                p_pl_layout = m_pl_data[UM3D_I].p_pipeline_layout;
                break;

            case DAS_ASSET_MODE_3D_TEXTURE_MAPPED:
                p_pl = &m_pl_data[TM3D_I].pipeline;
                p_pl_layout = m_pl_data[TM3D_I].p_pipeline_layout;
                break;

            default:
                UNDEFINED_ASSET_MODE(asset.name, asset.uuid);
                break;
            }

            vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, *p_pl);

            // Pipeline layout is necessary for binding descriptor sets
            // That is why it is returned with this method call
            return p_pl_layout;
        }


        /// Set miscellanious data arrays 
        void __vk_DrawCaller::setMiscData (
            const std::array<__vk_PipelineData, PIPELINE_C> &pl_data, 
            const std::vector<VkFramebuffer> &fb
        ) {
            m_pl_data = pl_data;
            m_framebuffers = fb;
        }


        /// Allocate memory for commandbuffers
        void __vk_DrawCaller::allocateCmdBuffers (
            VkDevice device, 
            VkQueue g_queue, 
            VkRenderPass renderpass, 
            VkExtent2D ext,
            dengMath::vec4<deng_vec_t> background,
            const __vk_BufferData &bd,
            const deng_bool_t use_lvl_zero
        ) {
            m_cmd_bufs.resize(m_framebuffers.size());

            // Set up commandbuffer allocate info
            VkCommandBufferAllocateInfo cmd_buf_alloc_info{};
            cmd_buf_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buf_alloc_info.commandPool = m_cmd_pool;
            cmd_buf_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buf_alloc_info.commandBufferCount = static_cast<deng_ui32_t>(m_cmd_bufs.size());

            // Allocate command buffers
            if(vkAllocateCommandBuffers(device, &cmd_buf_alloc_info, m_cmd_bufs.data())) 
                VK_DRAWCMD_ERR("failed to allocate command buffers");
            
            m_cmd_bufs.resize(__max_frame_c);
        }


        /// Record command buffers for drawing assets and optionally ui elements
        void __vk_DrawCaller::recordCmdBuffers (
            VkRenderPass renderpass,
            VkExtent2D ext,
            const dengMath::vec4<deng_vec_t> &background,
            const __vk_BufferData &bd,
            const deng_bool_t use_lvl_zero
        ) {
            size_t i, j;

            // Record each command buffer
            for(i = 0; i < m_cmd_bufs.size(); i++) {
                VkCommandBufferBeginInfo cmd_buf_info{};
                cmd_buf_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                // Begin recording command buffer
                if(vkBeginCommandBuffer(m_cmd_bufs.at(i), &cmd_buf_info) != VK_SUCCESS)
                    VK_DRAWCMD_ERR("failed to begin recording command buffers");

                // Set up renderpass begin info
                VkRenderPassBeginInfo renderpass_begininfo{};
                renderpass_begininfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderpass_begininfo.renderPass = renderpass;
                renderpass_begininfo.framebuffer = m_framebuffers[i];
                renderpass_begininfo.renderArea.offset = {0, 0};
                renderpass_begininfo.renderArea.extent = ext;

                // Set up clear values
                std::array<VkClearValue, 2> clear_values;
                clear_values[0].color = {{
                    background.first, 
                    background.second, 
                    background.third, 
                    background.fourth
                }};
                clear_values[1].depthStencil = {1.0f, 0};

                // Add clear values to renderpass begin info
                renderpass_begininfo.clearValueCount = 
                static_cast<deng_ui32_t>(clear_values.size());
                renderpass_begininfo.pClearValues = clear_values.data();
                
                // Start a new render pass for recording asset draw commands
                vkCmdBeginRenderPass(m_cmd_bufs.at(i), &renderpass_begininfo, 
                    VK_SUBPASS_CONTENTS_INLINE);

                    // Iterate through every asset, bind resources and issue an index draw to commandbuffer
                    for(j = 0; j < m_assets.size(); j++) {
                        RegType reg_asset = m_reg.retrieve(m_assets[j], 
                            DENG_SUPPORTED_REG_TYPE_ASSET, NULL);

                        RegType reg_vk_asset = m_reg.retrieve(reg_asset.asset.vk_id,
                            DENG_SUPPORTED_REG_TYPE_VK_ASSET, NULL);

                        if(reg_asset.asset.is_shown) {
                            __bindAssetResources(reg_asset.asset, m_cmd_bufs.at(i), bd);
                            VkPipelineLayout *p_pl_layout = __bindAssetPipeline(reg_asset.asset, m_cmd_bufs.at(i));

                            vkCmdBindDescriptorSets(m_cmd_bufs.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                *p_pl_layout, 0, 1, &reg_vk_asset.vk_asset.desc_sets[i], 0, NULL);

                            vkCmdDrawIndexed(m_cmd_bufs.at(i), static_cast<deng_ui32_t>(reg_asset.asset.indices.n), 
                                1, 0, 0, 0);
                        }
                    }

                    // Check if ui elements should be drawn
                    if(m_p_ui_data) {
                        LOG("UI entity count: " + std::to_string(m_p_ui_data->entities.size()));
                        for(j = 0; j < m_p_ui_data->entities.size(); j++) {
                            __bindUIElementResources(&m_p_ui_data->entities[j], m_cmd_bufs.at(i), bd);

                            vkCmdBindPipeline(m_cmd_bufs.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pl_data[UI_I].pipeline);

                            vkCmdBindDescriptorSets(m_cmd_bufs.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                *m_pl_data[UI_I].p_pipeline_layout, 0, 1, &m_ui_sets[i], 0, NULL);

                            LOG("Index count: " + std::to_string(m_p_ui_data->entities[j].ind_c));
                            vkCmdDrawIndexed(m_cmd_bufs.at(i), m_p_ui_data->entities[j].ind_c, 1, 0, 0, 0);
                        }
                    }

                // End render pass
                vkCmdEndRenderPass(m_cmd_bufs.at(i));
                

                // Stop recording commandbuffer
                if(vkEndCommandBuffer(m_cmd_bufs.at(i)) != VK_SUCCESS)
                    VK_DRAWCMD_ERR("failed to end recording command buffer");
            }
        }


        /// Getter and setter methods
        void __vk_DrawCaller::setUIDataPtr(__ImGuiData *p_gui) { m_p_ui_data = p_gui; }
        VkCommandPool __vk_DrawCaller::getComPool() { return m_cmd_pool; }
        const std::vector<VkCommandBuffer> &__vk_DrawCaller::getComBufs() { 
            return m_cmd_bufs;
        }
    }   
}
