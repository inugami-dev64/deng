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


#define __RM_CPP
#include <deng/vulkan/rm.h>

namespace deng {
    namespace vulkan {
        extern deng_ui32_t __max_frame_c;       

        /*********************************************************/
        /********** __vk_ResourceManager class methods ***********/
        /*********************************************************/

        __vk_ResourceManager::__vk_ResourceManager (
            VkDevice device, 
            VkPhysicalDevice gpu, 
            VkExtent2D extent,
            VkSampleCountFlagBits sample_c, 
            VkRenderPass renderpass, 
            VkCommandPool cmd_pool,
            VkQueue g_queue,
            const std::vector<VkImageView> &sc_img_views,
            deng::__GlobalRegistry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures,
            VkFormat sc_color_format,
            const VkPhysicalDeviceLimits &gpu_limits) : 
            __vk_BufferManager(device, gpu, gpu_limits, assets, reg),
            __vk_TextureManager(device, gpu, cmd_pool, g_queue, 
                textures, reg, m_buffer_data) 
        {
            m_sample_c = sample_c;
            // Allocate memory for new uniform buffers
            initUniformBuffer(device, gpu, cmd_pool, g_queue);

            // Create new color and depth resources
            __mkColorResources(device, gpu, extent, sc_color_format);
            __mkDepthResources(device, gpu, extent); 

            // Allocate frame buffers
            __mkFrameBuffers(device, renderpass, extent, sc_img_views);
        }




        /// Create new framebuffers 
        void __vk_ResourceManager::__mkFrameBuffers (
            VkDevice &device, 
            VkRenderPass &renderpass, 
            VkExtent2D &extent, 
            const std::vector<VkImageView> &sc_img_views
        ) {
            size_t index;
            m_framebuffers.resize(sc_img_views.size());
            std::array<VkImageView, 3> attachments;

            for(index = 0; index < sc_img_views.size(); index++) {
                attachments = {m_color_image_view, m_depth_image_view, sc_img_views[index]};

                VkFramebufferCreateInfo framebuffer_createinfo{};
                framebuffer_createinfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_createinfo.renderPass = renderpass;
                framebuffer_createinfo.attachmentCount = static_cast<deng_ui32_t>(attachments.size());
                framebuffer_createinfo.pAttachments = attachments.data();
                framebuffer_createinfo.width = extent.width;
                framebuffer_createinfo.height = extent.height;
                framebuffer_createinfo.layers = 1;

                if(vkCreateFramebuffer(device, &framebuffer_createinfo, NULL, &m_framebuffers[index]) != VK_SUCCESS)
                    VK_RES_ERR("failed to create framebuffer!");
                
                else LOG("Framebuffer successfully created");
            }
        }


        /// Create color resources for multisampling 
        void __vk_ResourceManager::__mkColorResources (
            VkDevice &device,
            VkPhysicalDevice &gpu,
            VkExtent2D &extent,
            VkFormat sc_color_format
        ) {
            // Create a new Vulkan image handle
            VkMemoryRequirements mem_req = __vk_ImageCreator::makeImage(device, gpu, m_color_image, extent.width, 
                extent.height, 1, sc_color_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | 
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, m_sample_c);

            // Allocate memory for the image handle
            __vk_BufferCreator::allocateMemory(device, gpu, mem_req.size, m_color_image_mem,
                mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            // Bind the memory with the image handle
            vkBindImageMemory(device, m_color_image, m_color_image_mem, 0);

            // Create image view
            VkImageViewCreateInfo image_view_createinfo = __vk_ImageCreator::getImageViewInfo (
                m_color_image, sc_color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);

            if(vkCreateImageView(device, &image_view_createinfo, NULL, &m_color_image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create color image view");
        }


        /// Create depth image buffers for depth buffering 
        void __vk_ResourceManager::__mkDepthResources (
            VkDevice &device, 
            VkPhysicalDevice &gpu, 
            VkExtent2D &extent
        ) {
            // Create an VkImage instance for depth buffers
            VkMemoryRequirements mem_req = __vk_ImageCreator::makeImage (
                device, gpu, m_depth_image, extent.width, extent.height, 
                1, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, 
                VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, m_sample_c);
            
            // Allocate memory for image buffer
            __vk_BufferCreator::allocateMemory(device, gpu, 
                mem_req.size, m_depth_image_mem, mem_req.memoryTypeBits, 
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            // Bind the image buffer with its memory
            vkBindImageMemory(device, m_depth_image, 
                m_depth_image_mem, 0);

            // Create image view createinfo
            VkImageViewCreateInfo image_view_createinfo = __vk_ImageCreator::getImageViewInfo (
                m_depth_image, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

            // Create new image view
            if(vkCreateImageView(device, &image_view_createinfo, NULL, &m_depth_image_view) != VK_SUCCESS)
                VK_RES_ERR("failed to create depth image view!");
        }


        /// __vk_ResourceManager class getters
        __vk_BufferData &__vk_ResourceManager::getBD() { return m_buffer_data; }
        std::vector<VkFramebuffer> __vk_ResourceManager::getFB() { return m_framebuffers; }
        VkImage __vk_ResourceManager::getDepImg() { return m_depth_image; }
        VkDeviceMemory __vk_ResourceManager::getDepImgMem() { return m_depth_image_mem; }
        VkImageView __vk_ResourceManager::getDepImgView() { return m_depth_image_view; }
        VkImage __vk_ResourceManager::getColorImg() { return m_color_image; }
        VkDeviceMemory __vk_ResourceManager::getColorImgMem() { return m_color_image_mem; }
        VkImageView __vk_ResourceManager::getColorImgView() { return m_color_image_view; }

        void __vk_ResourceManager::setUIDataPtr(__ImGuiData *p_gui) { __vk_BufferManager::setUIDataPtr(p_gui); };
    }
}
