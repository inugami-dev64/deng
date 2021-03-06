// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetLayoutCreator.h - Vulkan descriptor set layout creator class header
// author: Karl-Mihkel Ott


#ifndef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H
#define VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_H


#ifdef VULKAN_DESCRIPTOR_SET_LAYOUT_CREATOR_CPP
    #include <vector>
    #include <string>
    #include <cstring>
    #include <unordered_map>
#ifdef _DEBUG
    #include <iostream>
#endif
    #include <vulkan/vulkan.h>

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>

    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Api.h>
    #include <Window.h>
    #include <Renderer.h>
#endif


namespace DENG {

    namespace Vulkan {

        class DescriptorSetLayoutCreator {
            private:
                VkDevice m_device;
                ShaderModule const *m_shader_module;
                VkDescriptorSetLayout m_per_shader_descriptor_set_layout = VK_NULL_HANDLE;
                VkDescriptorSetLayout m_per_mesh_descriptor_set_layout = VK_NULL_HANDLE;

            private:
                void _CreatePerShaderDescriptorSetLayout();
                void _CreatePerMeshDescriptorSetLayout();

            public:
                DescriptorSetLayoutCreator(VkDevice _dev, const ShaderModule &_module);
                DescriptorSetLayoutCreator(const DescriptorSetLayoutCreator &_dslc) = delete;
                DescriptorSetLayoutCreator(DescriptorSetLayoutCreator &&_dslc) noexcept;
                ~DescriptorSetLayoutCreator();

                DescriptorSetLayoutCreator &operator=(const DescriptorSetLayoutCreator &_dslc) noexcept;
                DescriptorSetLayoutCreator &operator=(DescriptorSetLayoutCreator &&_dslc) noexcept;

                inline VkDescriptorSetLayout GetPerShaderDescriptorSetLayout() { return m_per_shader_descriptor_set_layout; }
                inline VkDescriptorSetLayout GetPerMeshDescriptorSetLayout() { return m_per_mesh_descriptor_set_layout; }
        };
    }
}

#endif
