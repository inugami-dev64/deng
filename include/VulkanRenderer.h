// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanRenderer.h - Vulkan renderer class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H


#ifdef VULKAN_RENDERER_CPP
    #include <string>
    #include <vector>
    #include <array>
    #include <cstring>
    #include <cmath>
    #include <utility>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>
    #include <shaderc/shaderc.hpp>

    #include <libdas/include/Api.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/TextureReader.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <Window.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Renderer.h>
#endif

#define DEFAULT_VERTICES_SIZE   2048
#define DEFAULT_INDICES_SIZE    2048

#include <VulkanHelpers.h>
#include <VulkanInstanceCreator.h>
#include <VulkanSwapchainCreator.h>
#include <VulkanPipelineCreator.h>
#include <VulkanDescriptorPoolCreator.h>
#include <VulkanDescriptorSetLayoutCreator.h>
#include <VulkanUniformBufferAllocator.h>
#include <VulkanDescriptorSetsCreator.h>



namespace DENG {

    class DENG_API VulkanRenderer : public Renderer {
        private:
            Vulkan::InstanceCreator *mp_instance_creator;
            Vulkan::SwapchainCreator *mp_swapchain_creator;
            std::vector<Vulkan::PipelineCreator> m_pipeline_creators;
            std::vector<Vulkan::DescriptorSetLayoutCreator> m_descriptor_set_layout_creators;
            std::vector<Vulkan::DescriptorPoolCreator> m_descriptor_pool_creators;
            std::vector<Vulkan::DescriptorSetsCreator> m_descriptor_sets_creators;
            Vulkan::UniformBufferAllocator *mp_ubo_allocator;
            
            // locally managed vulkan resources
            VkSampleCountFlagBits m_sample_count = VK_SAMPLE_COUNT_2_BIT; // tmp
            VkDeviceSize m_vertices_size = DEFAULT_VERTICES_SIZE;
            VkDeviceSize m_indices_size = DEFAULT_INDICES_SIZE;
            VkBuffer m_main_buffer = VK_NULL_HANDLE;
            VkDeviceMemory m_main_memory = VK_NULL_HANDLE;

            VkCommandPool m_command_pool = VK_NULL_HANDLE;
            std::vector<VkSemaphore> m_render_finished_semaphores;
            std::vector<VkSemaphore> m_image_available_semaphores;
            std::vector<VkFence> m_flight_fences;
            std::vector<VkCommandBuffer> m_cmd_buffers;

            // color and depth image resources
            VkImage m_color_image = VK_NULL_HANDLE;
            VkDeviceMemory m_color_image_memory = VK_NULL_HANDLE;
            VkImageView m_color_image_view = VK_NULL_HANDLE;

            VkImage m_depth_image = VK_NULL_HANDLE;
            VkDeviceMemory m_depth_image_memory = VK_NULL_HANDLE;
            VkImageView m_depth_image_view = VK_NULL_HANDLE;

            // framebuffers and command buffers
            std::vector<VkFramebuffer> m_framebuffers;
            std::vector<VkCommandBuffer> m_command_buffers;
            uint32_t m_current_frame = 0;

            std::vector<Vulkan::TextureData> m_vulkan_texture_handles;

        private:
            void _CreateCommandPool();
            void _CreateSemaphores();
            void _AllocateBufferResources();
            void _ReallocateBufferResources(VkDeviceSize _old_vert_size, VkDeviceSize _old_ind_size);
            void _CreateColorResources();
            void _CreateDepthResources();
            void _CreateFrameBuffers();
            void _AllocateCommandBuffers();
            void _RecordCommandBuffers();
            void _CreateMipmaps(VkImage _img, uint32_t _width, uint32_t _height, uint32_t _mip_levels);
            void _CreateTextureSampler(Vulkan::TextureData &_tex, uint32_t _mip_levels);


        public:
            VulkanRenderer(const Window &_win);
            ~VulkanRenderer();

            virtual uint32_t PushTextureFromFile(const DENG::TextureReference &_tex, const std::string &_file_name) override;
            virtual uint32_t PushTextureFromMemory(const DENG::TextureReference &_tex, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) override;

            virtual void LoadShaders() override;
            virtual void UpdateUniform(char *_raw_data, uint32_t _shader_id, uint32_t _ubo_id) override;
            virtual void UpdateVertexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void UpdateIndexBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset = 0) override;
            virtual void ClearFrame() override;
            virtual void RenderFrame() override;
    };
}

#endif
