// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanPipelineCreator.h - Vulkan pipeline creation class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_PIPELINE_CREATOR_H
#define VULKAN_PIPELINE_CREATOR_H

#ifdef VULKAN_PIPELINE_CREATOR_CPP
    #include <string>
    #include <fstream>
    #include <vector>
    #include <array>
    #include <vulkan/vulkan.h>
    #include <shaderc/shaderc.hpp>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
#endif


namespace DENG {
    namespace Vulkan {
        
        class PipelineCreator {
            private:
                VkDevice m_device;
                VkViewport m_viewport = {};
                VkRect2D m_scissor = {};
                VkExtent2D m_ext = {};
                VkSampleCountFlagBits m_samples = {};

                std::vector<VkPipelineShaderStageCreateInfo> m_shader_stage_createinfos = {};
                std::vector<VkShaderModule> m_shader_modules = {};
                std::vector<VkVertexInputBindingDescription> m_input_binding_desc = {};
                std::vector<VkVertexInputAttributeDescription> m_input_attr_descs = {};


                VkPipelineVertexInputStateCreateInfo    m_vert_input_create_info = {};
                VkPipelineInputAssemblyStateCreateInfo  m_input_asm_createinfo = {};
                VkPipelineViewportStateCreateInfo       m_viewport_state_createinfo = {};
                VkPipelineRasterizationStateCreateInfo  m_rasterization_createinfo = {};
                VkPipelineMultisampleStateCreateInfo    m_multisample_createinfo = {};
                VkPipelineColorBlendAttachmentState     m_colorblend_attachment = {};
                VkPipelineDepthStencilStateCreateInfo   m_depth_stencil = {};
                VkPipelineColorBlendStateCreateInfo     m_colorblend_state_createinfo = {};

                VkPipelineDynamicStateCreateInfo        m_dynamic_state_createinfo = {};
                std::vector<VkDynamicState> m_dynamic_states;

                std::vector<uint32_t> m_vertex_bin;
                std::vector<uint32_t> m_geometry_bin;
                std::vector<uint32_t> m_fragment_bin;

                std::vector<VkPipelineLayout> m_pipeline_layouts;
                std::vector<VkPipeline> m_pipelines;
                VkRenderPass m_render_pass = {};

            private:
                void _FindInputBindingDescriptions(ShaderModule *_module);
                void _FindVertexInputAttributeDescriptions(ShaderModule *_module);
                VkShaderModule _CreateShaderModule(std::vector<uint32_t> &_bin);
                void _CompileShader(std::vector<uint32_t> &_target, const std::string &_src, shaderc_shader_kind _kind);
                void _CheckAndCompileShaderSources(ShaderModule *_module);
                std::string _ReadShaderSource(const std::string &_file_name);
                void _CreatePipelineLayouts(const std::vector<VkDescriptorSetLayout> &_desc_set_layouts);
                VkGraphicsPipelineCreateInfo _GeneratePipelineCreateInfo(ShaderModule *_module, VkPipelineLayout &_layout);

            public:
                PipelineCreator(VkDevice _dev, VkRenderPass _render_pass, VkExtent2D _ext, VkSampleCountFlagBits _samples, const std::vector<VkDescriptorSetLayout> &_desc_set_layouts, const std::vector<ShaderModule*> &_modules);
                ~PipelineCreator();

                inline VkPipelineLayout GetPipelineLayoutById(uint32_t _id) { return m_pipeline_layouts[_id]; }
                inline VkPipeline GetPipelineById(uint32_t _id) { return m_pipelines[_id]; }
        };
    }
}

#endif
