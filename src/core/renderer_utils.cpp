#include "deng_core.h"

namespace deng {
    
    dengBool HardwareSpecs::getExtensionSupport(const VkPhysicalDevice &gpu, const char *p_extenstion_name) {
        LOG("Finding support for extension " + std::string(p_extenstion_name));
        uint32_t local_extension_count;
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &local_extension_count, nullptr);
        
        std::vector<VkExtensionProperties> local_all_extensions(local_extension_count);
        vkEnumerateDeviceExtensionProperties(gpu, nullptr, &local_extension_count, local_all_extensions.data());
        
        for(VkExtensionProperties &extension : local_all_extensions) {
            const char *local_current_extenstion_name = extension.extensionName;
            LOG("Supported: " + std::string(local_current_extenstion_name));
            if(std::string(local_current_extenstion_name) == std::string(p_extenstion_name)) {
                LOG("Detected support for " + std::string(local_current_extenstion_name));
                return DENG_TRUE;
            }
        }

        return DENG_FALSE;
    }

    uint32_t HardwareSpecs::getMemoryType(const VkPhysicalDevice &gpu, const uint32_t &type_filter, const VkMemoryPropertyFlags &properties) {
        VkPhysicalDeviceMemoryProperties local_memory_properties;
        vkGetPhysicalDeviceMemoryProperties(gpu, &local_memory_properties);

        for(uint32_t i = 0; i < local_memory_properties.memoryTypeCount; i++) {
            if(type_filter & (1 << i) && (local_memory_properties.memoryTypes[i].propertyFlags & properties)) {
                LOG("Vertex buffer mem type: " + std::to_string(i));
                return i;
            }
        }

        ERR("Failed to find suitable memory type");
    }

    uint32_t HardwareSpecs::getDeviceScore(const VkPhysicalDevice &gpu, std::vector<const char*> &required_extenstions) {
        uint32_t score = 0;
        VkPhysicalDeviceFeatures local_device_features;
        VkPhysicalDeviceProperties local_device_properties;

        vkGetPhysicalDeviceProperties(gpu, &local_device_properties);
        vkGetPhysicalDeviceFeatures(gpu, &local_device_features);

        if(local_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
        
        if(local_device_features.fillModeNonSolid == true) score += 500;

        score += local_device_properties.limits.maxImageDimension2D;
        score += local_device_properties.limits.maxImageDimension3D;
        score += local_device_properties.limits.maxMemoryAllocationCount;
        score += local_device_properties.limits.maxVertexOutputComponents;

        if(!local_device_features.geometryShader) return 0;
        if(!local_device_features.samplerAnisotropy) return 0;

        for(const char* extenstion_name : required_extenstions) {
            if(HardwareSpecs::getExtensionSupport(gpu, extenstion_name) != DENG_TRUE) {
                LOG("Required extension: " + std::string(extenstion_name) + " is not supported!");
                return 0;
            }
        }

        return score;
    }

    bool QueueFamilies::findGraphicsFamily(VkPhysicalDevice &gpu) {
        uint32_t family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_family_properties(family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &family_count, queue_family_properties.data());

        for(uint32_t i = 0; i < family_count; i++) {
            if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                this->m_graphics_family_index = i;
                LOG("Graphics queue family index: " + std::to_string(this->getGraphicsFamily()));
                return true;
            }
        }

        return false;
    }

    uint32_t QueueFamilies::getGraphicsFamily() {
        return this->m_graphics_family_index;
    }

    bool QueueFamilies::findPresentSupportFamily(VkPhysicalDevice &gpu, VkSurfaceKHR &surface) {
        uint32_t family_count = 0;
        VkBool32 support = false;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &family_count, nullptr);
        std::vector<VkQueueFamilyProperties> family_properties(family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &family_count, family_properties.data());

        for(uint32_t i = 0; i < family_count; i++) {
            vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &support);
            if(support) {
                if(this->m_graphics_family_index != i) {
                    this->m_present_family_index = i; 
                    LOG("Presentation queue family index: " + std::to_string(this->m_present_family_index));
                    return true;
                }
            }
        }
        return false;
    }

    uint32_t QueueFamilies::getPresentFamily() {
        return this->m_present_family_index;
    }


    PipelineCreator::PipelineCreator(PipelineData *p_pipeline_data, VkDevice *device, dengUtils::FileManager *filemanager, VkExtent2D *extent, VkRenderPass *renderpass) {
        this->m_p_pipeline_data = p_pipeline_data;
        this->m_p_device = device;
        this->m_p_fm = filemanager;
        this->m_p_extent = extent;
        this->m_p_renderpass = renderpass;
    }

    VkShaderModule PipelineCreator::getShaderModule(std::vector<char> &shader_bins) {
        VkShaderModuleCreateInfo local_createinfo{};
        local_createinfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        local_createinfo.codeSize = shader_bins.size();
        local_createinfo.pCode = reinterpret_cast<const uint32_t*>(shader_bins.data());
        VkShaderModule local_shader_module;
        if(vkCreateShaderModule(*this->m_p_device, &local_createinfo, nullptr, &local_shader_module) != VK_SUCCESS) {
            ERR("Failed to create shader module!");
        }

        return local_shader_module;
    }

    VkVertexInputBindingDescription PipelineCreator::getBindingDesc() {
        VkVertexInputBindingDescription local_input_binding_desc{};
        local_input_binding_desc.binding = 0;

        switch (this->m_p_pipeline_data->pipeline_type)
        {
        case DENG_PIPELINE_TYPE_OBJECT_BASED:
            local_input_binding_desc.stride = sizeof(dengUtils::ObjVertexData);
            break;
        
        case DENG_PIPELINE_TYPE_SPECIFIED:
            local_input_binding_desc.stride = sizeof(dengUtils::SpecifiedVertexData);
            break;

        case DENG_PIPELINE_TYPE_UI:
            local_input_binding_desc.stride = sizeof(dengUtils::UIVerticesData);
            break;
        
        default:
            break;
        }

        return local_input_binding_desc;
    } 

    std::vector<VkVertexInputAttributeDescription> PipelineCreator::getAttributeDesc() {
        std::vector<VkVertexInputAttributeDescription> local_input_attr_desc{};
        local_input_attr_desc.resize(2);

        switch (this->m_p_pipeline_data->pipeline_type)
        {
        case DENG_PIPELINE_TYPE_OBJECT_BASED:
            local_input_attr_desc[0].binding = 0;
            local_input_attr_desc[0].location = 0;
            local_input_attr_desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            local_input_attr_desc[0].offset = offsetof(dengUtils::ObjVertexData, position_vec);

            local_input_attr_desc[1].binding = 0;
            local_input_attr_desc[1].location = 1;
            local_input_attr_desc[1].format = VK_FORMAT_R32G32_SFLOAT;
            local_input_attr_desc[1].offset = offsetof(dengUtils::ObjVertexData, texture_vec);
            break;

        case DENG_PIPELINE_TYPE_SPECIFIED:
            local_input_attr_desc[0].binding = 0;
            local_input_attr_desc[0].location = 0;
            local_input_attr_desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            local_input_attr_desc[0].offset = offsetof(dengUtils::SpecifiedVertexData, position_vec);

            local_input_attr_desc[1].binding = 0;
            local_input_attr_desc[1].location = 1;
            local_input_attr_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            local_input_attr_desc[1].offset = offsetof(dengUtils::SpecifiedVertexData, color_vec);
            break;

        case DENG_PIPELINE_TYPE_UI:
            local_input_attr_desc[0].binding = 0;
            local_input_attr_desc[0].location = 0;
            local_input_attr_desc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            local_input_attr_desc[0].offset = offsetof(dengUtils::UIVerticesData, position_vec);
            
            local_input_attr_desc[1].binding = 0;
            local_input_attr_desc[1].location = 1;
            local_input_attr_desc[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            local_input_attr_desc[1].offset = offsetof(dengUtils::UIVerticesData, color_vec);
            break;
        
        default:
            break;
        }

        return local_input_attr_desc;
    }

    VkGraphicsPipelineCreateInfo PipelineCreator::getGraphicsPipelineInfo(const std::string &vert_shader, const std::string &frag_shader, const char *p_shader_module_name,
    const VkPolygonMode &polygon_mode, const VkCullModeFlagBits &cull_mode, const VkFrontFace &front_face, const VkPrimitiveTopology &primitive_topology, const dengBool &add_depth_stencil, const dengBool &add_color_blend, const uint32_t &subpass_index) {
        
        std::vector<char> vert_shader_binary_vector;
        std::vector<char> frag_shader_binary_vector;
        this->m_p_fm->getFileContents(vert_shader, &vert_shader_binary_vector, nullptr);
        this->m_p_fm->getFileContents(frag_shader, &frag_shader_binary_vector, nullptr);

        this->m_shader_modules[0] = this->getShaderModule(vert_shader_binary_vector);
        this->m_shader_modules[1] = this->getShaderModule(frag_shader_binary_vector);

        VkPipelineShaderStageCreateInfo local_vertex_shader_stage_createinfo{};
        local_vertex_shader_stage_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        local_vertex_shader_stage_createinfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        local_vertex_shader_stage_createinfo.module = this->m_shader_modules[0];
        local_vertex_shader_stage_createinfo.pName = p_shader_module_name;

        VkPipelineShaderStageCreateInfo local_frag_shader_stage_createinfo{};
        local_frag_shader_stage_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        local_frag_shader_stage_createinfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        local_frag_shader_stage_createinfo.module = this->m_shader_modules[1];
        local_frag_shader_stage_createinfo.pName = p_shader_module_name;

        this->m_shader_stage_createinfos = {local_vertex_shader_stage_createinfo, local_frag_shader_stage_createinfo};

        this->m_input_binding_descriptor = this->getBindingDesc();
        this->m_input_attribute_descriptors = this->getAttributeDesc();

        this->m_vertex_input_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        this->m_vertex_input_createinfo.vertexBindingDescriptionCount = 1;
        this->m_vertex_input_createinfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(this->m_input_attribute_descriptors.size());
        this->m_vertex_input_createinfo.pVertexBindingDescriptions = &this->m_input_binding_descriptor;
        this->m_vertex_input_createinfo.pVertexAttributeDescriptions = this->m_input_attribute_descriptors.data();

        this->m_input_assembly_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        this->m_input_assembly_createinfo.topology = primitive_topology;
        this->m_input_assembly_createinfo.primitiveRestartEnable = VK_FALSE;

        this->m_viewport.x = 0.0f;
        this->m_viewport.y = 0.0f;
        this->m_viewport.width = static_cast<float>(this->m_p_extent->width);
        this->m_viewport.height = static_cast<float>(this->m_p_extent->height);
        this->m_viewport.minDepth = 0.0f;
        this->m_viewport.maxDepth = 1.0f;

        this->m_scissor.offset = {0, 0};
        this->m_scissor.extent = *this->m_p_extent;

        this->m_viewport_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        this->m_viewport_state_createinfo.viewportCount = 1;
        this->m_viewport_state_createinfo.pViewports = &this->m_viewport;
        this->m_viewport_state_createinfo.scissorCount = 1;
        this->m_viewport_state_createinfo.pScissors = &this->m_scissor;

        this->m_rasterization_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        this->m_rasterization_createinfo.depthClampEnable = VK_FALSE;
        this->m_rasterization_createinfo.rasterizerDiscardEnable = VK_FALSE;

        this->m_rasterization_createinfo.polygonMode = polygon_mode;
        this->m_rasterization_createinfo.lineWidth = 1.0f;
        this->m_rasterization_createinfo.cullMode = cull_mode;
        if(cull_mode != VK_CULL_MODE_NONE) {
            this->m_rasterization_createinfo.frontFace = front_face;
            this->m_rasterization_createinfo.depthBiasEnable = VK_TRUE;
        }

        this->m_multisample_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        this->m_multisample_createinfo.sampleShadingEnable = VK_FALSE;
        this->m_multisample_createinfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        this->m_colorblend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        if(add_color_blend == DENG_TRUE) this->m_colorblend_attachment.blendEnable =  VK_TRUE;
        else this->m_colorblend_attachment.blendEnable = VK_FALSE;
        
        this->m_depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        if(add_depth_stencil == DENG_TRUE) {
            this->m_depth_stencil.depthTestEnable = VK_TRUE;
            this->m_depth_stencil.depthWriteEnable = VK_TRUE;
            this->m_depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
            this->m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
            this->m_depth_stencil.stencilTestEnable = VK_FALSE;
        }
        else {
            this->m_depth_stencil.depthTestEnable = VK_FALSE;
            this->m_depth_stencil.depthWriteEnable = VK_FALSE;
            this->m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
            this->m_depth_stencil.stencilTestEnable = VK_FALSE;
        }
        
        this->m_colorblend_state_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        this->m_colorblend_state_createinfo.logicOpEnable = VK_FALSE;
        this->m_colorblend_state_createinfo.attachmentCount = 1;
        this->m_colorblend_state_createinfo.pAttachments = &this->m_colorblend_attachment;

        VkGraphicsPipelineCreateInfo local_graphics_pipeline_createinfo{};
        local_graphics_pipeline_createinfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        local_graphics_pipeline_createinfo.stageCount = this->m_shader_stage_createinfos.size();
        local_graphics_pipeline_createinfo.pStages = this->m_shader_stage_createinfos.data();
        local_graphics_pipeline_createinfo.pVertexInputState = &this->m_vertex_input_createinfo;
        local_graphics_pipeline_createinfo.pInputAssemblyState = &this->m_input_assembly_createinfo;
        local_graphics_pipeline_createinfo.pViewportState = &this->m_viewport_state_createinfo;
        local_graphics_pipeline_createinfo.pColorBlendState = &this->m_colorblend_state_createinfo;
        local_graphics_pipeline_createinfo.pRasterizationState = &this->m_rasterization_createinfo;
        local_graphics_pipeline_createinfo.pMultisampleState = &this->m_multisample_createinfo;
        local_graphics_pipeline_createinfo.pDepthStencilState = &this->m_depth_stencil;
        local_graphics_pipeline_createinfo.layout = *this->m_p_pipeline_data->p_pipeline_layout;
        
        local_graphics_pipeline_createinfo.renderPass = *this->m_p_renderpass;
        local_graphics_pipeline_createinfo.subpass = subpass_index;
        local_graphics_pipeline_createinfo.basePipelineHandle = VK_NULL_HANDLE;

        return local_graphics_pipeline_createinfo;
    }

    PipelineCreator::~PipelineCreator() {
        vkDestroyShaderModule(*this->m_p_device, this->m_shader_modules[0], nullptr);
        vkDestroyShaderModule(*this->m_p_device, this->m_shader_modules[1], nullptr);
    }


    SwapChainDetails::SwapChainDetails(VkPhysicalDevice &gpu, VkSurfaceKHR &surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &this->m_capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, nullptr);

        if(format_count != 0) {
            this->m_formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &format_count, this->m_formats.data());
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, nullptr);

        if(present_mode_count != 0) {
            this->m_present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &present_mode_count, this->m_present_modes.data());
        }
        else {
            ERR("No present modes available!");
        }
    }

    VkSurfaceCapabilitiesKHR SwapChainDetails::getCapabilities() {
        return this->m_capabilities;
    }

    std::vector<VkSurfaceFormatKHR> SwapChainDetails::getFormats() {
        return this->m_formats;
    }

    std::vector<VkPresentModeKHR> SwapChainDetails::getPresentModes() {
        return this->m_present_modes;
    }

}