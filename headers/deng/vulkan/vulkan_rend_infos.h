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


#ifndef __VULKAN_REND_INFOS_H
#define __VULKAN_REND_INFOS_H

#define DENG_PIPELINE_COUNT 4

namespace deng {
    namespace vulkan {
        /* 
         * Struct for storing all vulkan device related information 
         */
        struct __vk_DeviceInfo {
            VkDevice m_device;
            deng::Window &m_win;
            VkPhysicalDevice m_gpu;
            __vk_DeviceInfo(deng::Window &win) : m_win(win) {} ;
        };


        /* 
         * Struct for storing all vk_ instance objects 
         */
        struct __vk_InstanceInfo {
            __vk_QueueManager m_qff;
            VkInstance m_instance;
            VkSurfaceKHR m_surface;
            VkDebugUtilsMessengerEXT m_debug_mes;
        };


        /* 
         * Struct for storing all vk_ swapchain related objects 
         */
        struct __vk_SwapChainInfo {
            VkRenderPass m_renderpass;
            VkSwapchainKHR m_swapchain;
            VkPresentModeKHR m_present_mode;
            VkExtent2D m_ext;
            VkFormat m_format;
            VkSurfaceFormatKHR m_surface_format;
            std::vector<VkImage> m_swapchain_images;
            std::vector<VkImageView> m_swapchain_image_views;
            __vk_SwapChainDetails *m_p_sc_details;
        };


        /* 
         * Contains information about certain pipeline used in DENG 
         */
        struct __vk_PipelineData {
            VkPipeline pipeline;
            deng_PipelineType pipeline_type;
            VkPipelineLayout *p_pipeline_layout;
        };


        /* 
         * Struct for storing all vk_ descriptor related objects 
         */
        struct __vk_DescriptorInfo {
            std::array<__vk_PipelineData, DENG_PIPELINE_COUNT> m_pipelines;
            VkPipelineLayout m_unmapped_pl;
            VkPipelineLayout m_tex_mapped_pl;
            VkDescriptorPool m_unmapped_desc_pool;
            VkDescriptorPool m_tex_mapped_desc_pool;
            VkDescriptorSetLayout m_unmapped_desc_set_layout;
            VkDescriptorSetLayout m_tex_mapped_desc_set_layout;
        };


        /* 
         * Struct for storing all vk_ resource allocation related objects 
         */
        struct __vk_ResourceInfo {
            std::vector<VkFramebuffer> m_framebuffers;
            VkImage m_depth_image;
            VkDeviceMemory m_depth_image_mem;
            VkImageView m_depth_image_view;

            VkSampleCountFlagBits m_sample_count;
            VkImage m_color_image;
            VkDeviceMemory m_color_image_mem;
            VkImageView m_color_image_view;

            __vk_BufferData m_buffer_data;
        };
    }
}

#endif
