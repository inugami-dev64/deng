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


#ifndef __RESOURCES_H
#define __RESOURCES_H

#define DENG_MAX_GPU_NAME_SIZE 256

#ifdef __RESOURCES_CPP
    #include <vector>
    #include <string>
    #include <string.h>
    #include <stdexcept>
    #include <vulkan/vulkan.h>

    #include <common/base_types.h>
    #include <common/err_def.h>
    #include <common/hashmap.h>
    #include <data/assets.h>
    #include <math/deng_math.h>
#endif


namespace deng {
    namespace vulkan {

        /*
         * Struct for assets in vulkan renderer
         */
        struct __vk_Asset {
            deng_Id base_id;
            deng_Id tex_uuid;
            deng_Id uuid;

            // Boolean flag for checking if descriptor sets have been created
            deng_bool_t is_desc;
            VkDescriptorSet *desc_sets;
            deng_ui32_t desc_c;
        };

        
        /* 
         * Struct for texture images
         * This struct contains textures and their Vulkan image buffers, views, samplers 
         * and memory objects. Additionally each texture image instance in DENG contains 
         * data to uniform color data in case texture mapping is disabled for certain assets
         */
        struct __vk_Texture {
            deng_Id base_id;
            deng_Id uuid;
            deng_bool_t is_buffered;
            VkImage image;
            VkImageView image_view;
            VkSampler sampler;
        };
        

        /*
         * Store information about graphics device
         * This struct __vk_is used to store information about graphics device properties
         */
        struct __vk_HwInfo {
            deng_ui32_t driver_version;
            deng_ui32_t vk_api_version;
            char *gpu_name;
        };


        /*
         * Get information about graphics hardware
         * This struct __vk_contains methods to get information about graphics hardware
         */
        struct __vk_HardwareSpecs {
            /*
             * Check if requested extension is supported
             * This method gets information about supported Vulkan extensions and checks  
             * if requested extension is one of them
             */
            static deng_bool_t getExtensionSupport (
                const VkPhysicalDevice &gpu, 
                char *ext_name
            );


            /*
             * Find device memory type
             * This method finds appropriate memory type by VkMemoryPropertyFlags
             */
            static deng_ui32_t getMemoryType (
                const VkPhysicalDevice &gpu, 
                deng_ui32_t type_filter, 
                VkMemoryPropertyFlags properties
            );


            /*
             * Find device score
             * Rate logical device based on its capabilities
             */
            static deng_ui32_t getDeviceScore (
                const VkPhysicalDevice &device, 
                std::vector<const char*> &exts
            );


            /*
             * Find information about graphics device
             * This method gets information about graphics device name, Vulkan api
             * version and driver version
             */
            static __vk_HwInfo getGpuInfo(const VkPhysicalDevice &gpu);
        };


        /*
         * Structure for storing uniform transformation related data
         */
        struct __vk_UniformTransformation {
            dengMath::mat4<deng_vec_t> transform;
            dengMath::mat4<deng_vec_t> view;
            deng_ui32_t no_perspective;
        };


        /*
         * Structure for storing uniform color data per each asset
         */
        struct __vk_UniformColorData {
            dengMath::vec4<deng_vec_t> color;
            deng_i32_t is_unmapped;
        };


        /* 
         * Struct for storing buffers This struct __vk_contains all buffers and their memory objects for
         * vulkan renderer
         */
        struct __vk_BufferData {
            VkBuffer staging_buffer;
            VkDeviceMemory staging_buffer_memory;

            // Device memory for mainly storing texture image data
            VkDeviceMemory img_memory;

            // The capacity of the image memory default size is able to hold 
            // four 512x512 bitmap images or 4194304 bytes or about 4MB
            VkDeviceSize img_memory_cap = 0;
            VkDeviceSize img_memory_offset = 0;

            VkBuffer main_buffer;
            VkDeviceMemory main_buffer_memory;
            VkDeviceSize main_buffer_size = 0;

            // Memory alignment for uniform data looks like that 
            // where n is swapchain image count and m is the amount of texture images
            // n * sizeof(UniformTransformation) bytes -- All transformation ubo data
            // n * m * sizeof(__vk_UniformColorData) bytes -- All asset specific color ubo data
            VkBuffer uniform_buffer;
            VkDeviceMemory uniform_buffer_mem;
            VkDeviceSize ubo_size = 0;
            VkDeviceSize ubo_cap = 0;
            VkDeviceSize ubo_asset_cap = 0;
        };

        
        /* 
         * Contains method for memory allocation 
         * This struct __vk_is inherited to ImageCreator and BufferCreator structs
         */
        struct __vk_MemoryAllocator {
            /*
             * Allocate graphics memory using Vulkan
             * This method is used to allocate memory for VkBuffer and VkImage objects
             */
            static void allocateMemory (
                const VkDevice &device, 
                const VkPhysicalDevice &gpu,  
                VkDeviceSize size, 
                VkDeviceMemory &memory,
                deng_ui32_t mem_type_bits, 
                VkMemoryPropertyFlags properties
            );
        };


        /* 
         * Container for all VkImage handling methods
         */
        struct __vk_ImageCreator : __vk_MemoryAllocator {
            /*
             * Create new VkImageCreateInfo instance
             * This method is used to simplify VkImageViewCreateInfo creation
             */
            static VkImageViewCreateInfo getImageViewInfo (
                const VkImage &image, 
                VkFormat format, 
                VkImageAspectFlags aspect_flags,
                deng_ui32_t mip_levels
            );

                
            /*
             * Create new VkImage instance
             * This method creates new VkImage instance and returns 
             * memory allocation requirements for this image
             */
            static VkMemoryRequirements makeImage (
                const VkDevice &device, 
                const VkPhysicalDevice &gpu, 
                VkImage &image, 
                deng_ui32_t width, 
                deng_ui32_t height, 
                deng_ui32_t mip_levels,
                VkFormat format, 
                VkImageTiling tiling, 
                VkImageUsageFlags usage,
                VkSampleCountFlagBits sample_c
            );

            /*
             * Transition VkImage from one layout to another
             * This method uses VkImageMemoryBarrier to transition image layout to new_layout
             */
            static void transitionImageLayout (
                const VkDevice &device, 
                const VkImage &image, 
                const VkCommandPool &commandpool, 
                const VkQueue &g_queue, 
                VkFormat format, 
                VkImageLayout old_layout, 
                VkImageLayout new_layout,
                deng_ui32_t mip_levels
            ); 

            /*
             * Copy VkBuffer to VkImage instance
             * This method copies data from src_buffer to dst_image
             */
            static void cpyBufferToImage (
                const VkDevice &device, 
                const VkCommandPool &commandpool, 
                const VkQueue &g_queue, 
                const VkBuffer &src_buffer, 
                const VkImage &dst_image, 
                deng_ui32_t width, 
                deng_ui32_t height
            );
        };

        
        /* 
         * Container for buffer handling methods
         */
        struct __vk_BufferCreator : __vk_MemoryAllocator {
            /*
             * Create new VkBuffer instance
             * This method creates new VkImage instance and returns 
             * memory allocation requirements for this image
             */
            static VkMemoryRequirements makeBuffer (
                const VkDevice &device, 
                const VkPhysicalDevice &gpu, 
                VkDeviceSize size, 
                VkBufferUsageFlags usage, 
                VkBuffer &buffer
            );
            
            /*
             * Copy data to graphics memory
             * This method copies size bytes of data from src_data to buf_mem using vkMapMemory
             */
            static void cpyToBufferMem (
                const VkDevice &device, 
                VkDeviceSize size, 
                void *src_data, 
                const VkDeviceMemory &buf_mem, 
                VkDeviceSize offset
            );


            /*
             * Copy buffer to other buffer
             * This method copies data from src_buffer to dst_buffer
             */
            static void cpyBufferToBuffer (
                const VkDevice &device, 
                const VkCommandPool &commandpool, 
                const VkQueue &g_queue, 
                const VkBuffer &src_buffer, 
                const VkBuffer &dst_buffer, 
                VkDeviceSize size, 
                VkDeviceSize src_offset,
                VkDeviceSize dst_offset
            );
        };


        /* 
         * Container for single use command buffers' allocation and deallocation
         */
        struct __vk_CommandBufferRecorder {
            /*
             * Start recording commandbuffer
             * This method allocates and begins recording commandbuffers meant for single use
             */
            static void beginCommandBufferSingleCommand (
                VkDevice device, 
                VkCommandPool commandpool, 
                VkCommandBuffer *p_commandbuffer
            );

            /* 
             * Finish recording commandbuffer
             * This method end commandbuffer recording, submits it into graphics queue
             * and frees commandbuffers
             */
            static void endCommandBufferSingleCommand (
                VkDevice device, 
                VkQueue graphics_queue, 
                VkCommandPool commandpool, 
                VkCommandBuffer *p_commandBuffer
            );
        };
    }
}

#endif
