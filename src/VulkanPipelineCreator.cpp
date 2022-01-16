// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanPipelineCreator.cpp - Vulkan pipeline creation class implementation
// author: Karl-Mihkel Ott

#define VULKAN_PIPELINE_CREATOR_CPP
#include <VulkanPipelineCreator.h>


namespace DENG {

    namespace Vulkan {

        PipelineCreator::PipelineCreator(VkDevice _dev, VkRenderPass _render_pass, VkExtent2D _ext, VkSampleCountFlagBits _samples, const std::vector<VkDescriptorSetLayout> &_desc_set_layouts, const std::vector<ShaderModule*> &_modules) :
            m_device(_dev), m_render_pass(_render_pass), m_ext(_ext), m_samples(_samples)
        {
            _CreatePipelineLayouts(_desc_set_layouts);

            // for each shader module create a VkPipeline instance
            for(size_t i = 0; i < _modules.size(); i++) {
                VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = _GeneratePipelineCreateInfo(_modules[i], m_pipeline_layouts[i]);
                
                // create a pipeline
                m_pipelines.push_back(VkPipeline{});
                if(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, NULL, &m_pipelines.back()) != VK_SUCCESS)
                    VK_PIPELINEC_ERR("failed to create a pipeline");
            }
        }


        PipelineCreator::~PipelineCreator() {
            for(size_t i = 0; i < m_shader_modules.size(); i++)
                vkDestroyShaderModule(m_device, m_shader_modules[i], NULL);
        } 


        std::string PipelineCreator::_ReadShaderSource(const std::string &_file_name) {
            std::ifstream file(_file_name, std::ios_base::binary);
            if(!file.good()) FILE_ERR(_file_name);

            file.seekg(0, std::ios_base::end);
            size_t len = file.tellg();
            file.seekg(0, std::ios_base::beg);

            std::vector<char> data(len);
            file.read(data.data(), data.size());

            return std::string(data.data(), data.size());
        }


        void PipelineCreator::_FindInputBindingDescriptions(ShaderModule *_module) {
            for(uint32_t i = 0; i < static_cast<uint32_t>(_module->strides.size()); i++) {
                m_input_binding_desc.push_back(VkVertexInputBindingDescription{});
                m_input_binding_desc.back().binding = i;
                m_input_binding_desc.back().stride = _module->strides[i];
                m_input_binding_desc.back().inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
        }


        void PipelineCreator::_FindVertexInputAttributeDescriptions(ShaderModule *_module) {
            for(uint32_t i = 0; i < static_cast<uint32_t>(_module->attributes.size()); i++) {
                m_input_attr_descs.push_back(VkVertexInputAttributeDescription{});
                m_input_attr_descs.back().binding = 0;
                m_input_attr_descs.back().location = i;

                switch(_module->attributes[i]) {
                    // single element attributes
                    case ATTRIBUTE_TYPE_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16_SINT;
                        break;

                    case ATTRIBUTE_TYPE_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32_SINT;
                        break;

                    // two element attributes
                    case ATTRIBUTE_TYPE_VEC2_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64G64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16_SINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC2_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC2_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32_SINT;
                        break;

                    // three element attributes
                    case ATTRIBUTE_TYPE_VEC3_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64G64B64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16B16_SINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC3_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC3_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32_SINT;
                        break;

                    // four element attributes
                    case ATTRIBUTE_TYPE_VEC4_FLOAT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32A32_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_DOUBLE:
                        m_input_attr_descs.back().format = VK_FORMAT_R64G64B64A64_SFLOAT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_SHORT:
                        m_input_attr_descs.back().format = VK_FORMAT_R16G16B16A16_SINT;
                        break;

                    case ATTRIBUTE_TYPE_VEC4_UINT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32A32_UINT;
                        break;
                    
                    case ATTRIBUTE_TYPE_VEC4_INT:
                        m_input_attr_descs.back().format = VK_FORMAT_R32G32B32A32_SINT;
                        break;
                }

                m_input_attr_descs.back().offset = _module->offsets[i];
            }
        }


        VkShaderModule PipelineCreator::_CreateShaderModule(std::vector<uint32_t> &_bin) {
            VkShaderModuleCreateInfo module_create_info = {};
            module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            module_create_info.codeSize = static_cast<uint32_t>(_bin.size() * sizeof(uint32_t));
            module_create_info.pCode = _bin.data();

            VkShaderModule shader_module;
            if(vkCreateShaderModule(m_device, &module_create_info, NULL, &shader_module) != VK_SUCCESS)
                VK_PIPELINEC_ERR("failed to create a shader module");

            return shader_module;
        }


        void PipelineCreator::_CompileShader(std::vector<uint32_t> &_target, const std::string &_src, shaderc_shader_kind _kind) {
            shaderc::Compiler compiler;
            shaderc::CompileOptions options;

            options.SetOptimizationLevel(shaderc_optimization_level_size);
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(_src, _kind, _src.c_str(), options);

            if(module.GetCompilationStatus() != shaderc_compilation_status_success) {
                std::cerr << module.GetErrorMessage();
                std::exit(EXIT_FAILURE);
            }

            _target = std::vector<uint32_t>(module.cbegin(), module.cend());
        }


        void PipelineCreator::_CheckAndCompileShaderSources(ShaderModule *_module) {
            if(_module->load_shaders_from_file) {
                _module->vertex_shader_src = _ReadShaderSource(_module->vertex_shader_file);
                std::string geometry_src;
                if(_module->geometry_shader_file != "")
                    geometry_src = _ReadShaderSource(_module->geometry_shader_file);
                std::string fragment_src = _ReadShaderSource(_module->fragment_shader_file);
            }

            _CompileShader(m_vertex_bin, _module->vertex_shader_src, shaderc_glsl_vertex_shader);
            if(_module->geometry_shader_src.size())
                _CompileShader(m_geometry_bin, _module->geometry_shader_src, shaderc_glsl_geometry_shader);
            _CompileShader(m_fragment_bin, _module->fragment_shader_src, shaderc_glsl_fragment_shader);
        }


        void PipelineCreator::_CreatePipelineLayouts(const std::vector<VkDescriptorSetLayout> &_desc_set_layouts) {
            for(const VkDescriptorSetLayout &layout : _desc_set_layouts) {
                VkPipelineLayoutCreateInfo pipeline_layout_createinfo = {};
                pipeline_layout_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipeline_layout_createinfo.pushConstantRangeCount = 0;
                pipeline_layout_createinfo.setLayoutCount = 1;
                pipeline_layout_createinfo.pSetLayouts = &layout;

                m_pipeline_layouts.push_back(VkPipelineLayout{});
                if(vkCreatePipelineLayout(m_device, &pipeline_layout_createinfo, NULL, &m_pipeline_layouts.back()) != VK_SUCCESS)
                    VK_PIPELINEC_ERR("failed to create a pipeline layout");
            }
        }


        VkGraphicsPipelineCreateInfo PipelineCreator::_GeneratePipelineCreateInfo(ShaderModule *_module, VkPipelineLayout &_layout) {
            _CheckAndCompileShaderSources(_module);

            // Create vertex and fragment shader modules
            m_shader_modules.reserve(3);
            m_shader_modules.push_back(_CreateShaderModule(m_vertex_bin)); 
            if(_module->geometry_shader_src.size())
                m_shader_modules.push_back(_CreateShaderModule(m_geometry_bin));
            m_shader_modules.push_back(_CreateShaderModule(m_fragment_bin));

            // Create vertex shader stage createinfo
            m_shader_stage_createinfos.push_back(VkPipelineShaderStageCreateInfo());
            m_shader_stage_createinfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            m_shader_stage_createinfos.back().stage = VK_SHADER_STAGE_VERTEX_BIT;
            m_shader_stage_createinfos.back().module = m_shader_modules[0];
            m_shader_stage_createinfos.back().pName = "main";

            // check if geomety shader stage create info needs to be 
            if(_module->geometry_shader_src.size()) {
                m_shader_stage_createinfos.push_back(VkPipelineShaderStageCreateInfo());
                m_shader_stage_createinfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                m_shader_stage_createinfos.back().stage = VK_SHADER_STAGE_GEOMETRY_BIT;
                m_shader_stage_createinfos.back().module = m_shader_modules[1];
                m_shader_stage_createinfos.back().pName = "main";

                m_shader_stage_createinfos.push_back(VkPipelineShaderStageCreateInfo());
                m_shader_stage_createinfos.back().module = m_shader_modules[2];
            }
            else {
                m_shader_stage_createinfos.push_back(VkPipelineShaderStageCreateInfo());
                m_shader_stage_createinfos.back().module = m_shader_modules[1];
            }
            
            // Create fragment shader stage createinfo
            m_shader_stage_createinfos.back().sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            m_shader_stage_createinfos.back().stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            m_shader_stage_createinfos.back().pName = "main";

            // Bind get binding descriptors and attribute descriptors for the current pipeline type
            _FindInputBindingDescriptions(_module);
            _FindVertexInputAttributeDescriptions(_module);

            // Set up vertex input createinfo object 
            m_vert_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            m_vert_input_create_info.vertexBindingDescriptionCount = static_cast<uint32_t>(m_input_binding_desc.size());
            m_vert_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_input_attr_descs.size());
            m_vert_input_create_info.pVertexBindingDescriptions = m_input_binding_desc.data();
            m_vert_input_create_info.pVertexAttributeDescriptions = m_input_attr_descs.data();

            // Set up input assembly createinfo object
            m_input_asm_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            m_input_asm_createinfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            m_input_asm_createinfo.primitiveRestartEnable = VK_FALSE;

            // Set viewport values
            m_viewport.x = 0.0f;
            m_viewport.y = 0.0f;
            m_viewport.width = static_cast<float>(m_ext.width);
            m_viewport.height = static_cast<float>(m_ext.height);
            m_viewport.minDepth = 0.0f;
            m_viewport.maxDepth = 1.0f;

            // Set scissor offset and extent values
            m_scissor.offset = {0, 0};
            m_scissor.extent = m_ext;

            // Set up viewport state createinfo object
            m_viewport_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            m_viewport_state_createinfo.viewportCount = 1;
            m_viewport_state_createinfo.pViewports = &m_viewport;
            m_viewport_state_createinfo.scissorCount = 1;
            m_viewport_state_createinfo.pScissors = &m_scissor;

            // Set up rasterization create info
            m_rasterization_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            m_rasterization_createinfo.depthClampEnable = VK_FALSE;
            m_rasterization_createinfo.rasterizerDiscardEnable = VK_FALSE;
            m_rasterization_createinfo.polygonMode = VK_POLYGON_MODE_FILL;
            m_rasterization_createinfo.lineWidth = 1.0f;
            m_rasterization_createinfo.cullMode = VK_CULL_MODE_NONE;

            //if(pl_flags.cull_mode != VK_CULL_MODE_NONE) {
                //m_rasterization_createinfo.frontFace = pl_flags.front_face;
                //m_rasterization_createinfo.depthBiasEnable = VK_TRUE;
            //}


            // Set up multisampling createinfo
            m_multisample_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            m_multisample_createinfo.sampleShadingEnable = VK_FALSE;
            m_multisample_createinfo.rasterizationSamples = m_samples;

            // Set colorblend options
            m_colorblend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            m_colorblend_attachment.blendEnable = VK_TRUE;
            m_colorblend_attachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;
            m_colorblend_attachment.blendEnable = VK_TRUE;
            m_colorblend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            m_colorblend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            m_colorblend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
            m_colorblend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            m_colorblend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            m_colorblend_attachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;
            
            // Check if depth stencil is enabled and if it is set the createinfo accordingly
            m_depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            m_depth_stencil.depthTestEnable = VK_TRUE;
            m_depth_stencil.depthWriteEnable = VK_TRUE;
            m_depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
            m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
            m_depth_stencil.stencilTestEnable = VK_FALSE;

            // Check if the dynamic pipeline state createinfo should be created
            //m_dynamic_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            //m_dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR);
            //m_dynamic_state_createinfo.dynamicStateCount = static_cast<uint32_t>(m_dynamic_states.size());
            //m_dynamic_state_createinfo.pDynamicStates = m_dynamic_states.data();
            
            // Set up colorblend state createinfo
            m_colorblend_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            m_colorblend_state_createinfo.logicOpEnable = VK_FALSE;
            m_colorblend_state_createinfo.attachmentCount = 1;
            m_colorblend_state_createinfo.pAttachments = &m_colorblend_attachment;

            // Set up graphics pipeline createinfo
            VkGraphicsPipelineCreateInfo graphics_pipeline_createinfo = {};
            graphics_pipeline_createinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            graphics_pipeline_createinfo.stageCount = static_cast<uint32_t>(m_shader_stage_createinfos.size());
            graphics_pipeline_createinfo.pStages = m_shader_stage_createinfos.data();
            graphics_pipeline_createinfo.pVertexInputState = &m_vert_input_create_info;
            graphics_pipeline_createinfo.pInputAssemblyState = &m_input_asm_createinfo;
            graphics_pipeline_createinfo.pViewportState = &m_viewport_state_createinfo;
            graphics_pipeline_createinfo.pColorBlendState = &m_colorblend_state_createinfo;
            graphics_pipeline_createinfo.pRasterizationState = &m_rasterization_createinfo;
            graphics_pipeline_createinfo.pMultisampleState = &m_multisample_createinfo;
            graphics_pipeline_createinfo.pDepthStencilState = &m_depth_stencil;
            //graphics_pipeline_createinfo.pDynamicState = &m_dynamic_state_createinfo;
            graphics_pipeline_createinfo.layout = _layout;

            graphics_pipeline_createinfo.renderPass = m_render_pass;
            // tmp
            graphics_pipeline_createinfo.subpass = 0;
            graphics_pipeline_createinfo.basePipelineHandle = VK_NULL_HANDLE;

            return graphics_pipeline_createinfo;
        }
    }
}