// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: VulkanDescriptorSetsCreator.cpp - Vulkan descriptor set creator class implementation
// author: Karl-Mihkel Ott

#define VULKAN_DESCRIPTOR_SETS_CREATOR_CPP
#include <VulkanDescriptorSetsCreator.h>

namespace DENG {

    namespace Vulkan {

        DescriptorSetsCreator::DescriptorSetsCreator(VkDevice _dev, uint32_t _sc_img_count, const ShaderModule &_module, uint32_t _mod_id, UniformBufferAllocator *_ubo_allocator, VkDescriptorPool _desc_pool, 
                                                     VkDescriptorSetLayout _desc_set_layout, std::vector<Vulkan::TextureData> &_textures) : m_device(_dev), m_swapchain_images_count(_sc_img_count), mp_ubo_allocator(_ubo_allocator),
                                                     m_descriptor_pool(_desc_pool), m_descriptor_set_layout(_desc_set_layout), m_shader_module(&_module), m_textures(&_textures), m_mod_id(_mod_id)
        {
            _CreateDescriptorSets();
        }


        DescriptorSetsCreator::DescriptorSetsCreator(DescriptorSetsCreator &&_dsc) : m_descriptor_sets(std::move(_dsc.m_descriptor_sets)),
            m_device(_dsc.m_device), m_swapchain_images_count(_dsc.m_swapchain_images_count), mp_ubo_allocator(_dsc.mp_ubo_allocator), 
            m_textures(_dsc.m_textures), m_descriptor_pool(_dsc.m_descriptor_pool), m_descriptor_set_layout(_dsc.m_descriptor_set_layout),
            m_shader_module(_dsc.m_shader_module), m_mod_id(_dsc.m_mod_id) {}


        // explicitly declared assignment operators
        DescriptorSetsCreator &DescriptorSetsCreator::operator=(const DescriptorSetsCreator &_dc) {
            m_descriptor_sets = _dc.m_descriptor_sets;
            m_device = _dc.m_device;
            m_swapchain_images_count = _dc.m_swapchain_images_count;
            mp_ubo_allocator = _dc.mp_ubo_allocator;
            m_textures = _dc.m_textures;
            m_descriptor_pool = _dc.m_descriptor_pool;
            m_descriptor_set_layout = _dc.m_descriptor_set_layout;
            m_shader_module = _dc.m_shader_module;
            m_mod_id = _dc.m_mod_id;

            return *this;
        }


        DescriptorSetsCreator &DescriptorSetsCreator::operator=(DescriptorSetsCreator &&_dc) {
            m_descriptor_sets = std::move(_dc.m_descriptor_sets);
            m_device = _dc.m_device;
            m_swapchain_images_count = _dc.m_swapchain_images_count;
            mp_ubo_allocator = _dc.mp_ubo_allocator;
            m_textures = _dc.m_textures;
            m_descriptor_pool = _dc.m_descriptor_pool;
            m_descriptor_set_layout = _dc.m_descriptor_set_layout;
            m_shader_module = _dc.m_shader_module;
            m_mod_id = _dc.m_mod_id;

            return *this;
        }


        void DescriptorSetsCreator::_CreateDescriptorSets() {
            std::vector<VkDescriptorSetLayout> layouts(m_swapchain_images_count, m_descriptor_set_layout);

            VkDescriptorSetAllocateInfo allocation_info = {};
            allocation_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocation_info.descriptorPool = m_descriptor_pool;
            allocation_info.descriptorSetCount = m_swapchain_images_count;
            allocation_info.pSetLayouts = layouts.data();

            m_descriptor_sets.resize((m_textures->size() > 0 ? m_textures->size() : 1) * static_cast<size_t>(m_swapchain_images_count));
            if(vkAllocateDescriptorSets(m_device, &allocation_info, m_descriptor_sets.data()))
                VK_DESC_ERR("failed to allocate descriptor sets");

            std::vector<VkDescriptorBufferInfo> buffer_infos(std::move(_FindBufferInfos()));
            std::vector<VkDescriptorImageInfo> img_infos(std::move(_FindImageInfos()));
            std::vector<VkWriteDescriptorSet> write_descs(std::move(_FindWriteDescriptorInfos(buffer_infos, img_infos)));

            vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(write_descs.size()), write_descs.data(), 0, nullptr);
        }


        std::vector<VkDescriptorBufferInfo> DescriptorSetsCreator::_FindBufferInfos() {
            std::vector<VkDescriptorBufferInfo> buffer_infos;
            buffer_infos.reserve(m_shader_module->ubo_data_layouts.size());

            // for each ubo data layout fill buffer info struct
            for(auto it = m_shader_module->ubo_data_layouts.begin(); it != m_shader_module->ubo_data_layouts.end(); it++) {
                const uint32_t id = static_cast<uint32_t>(it - m_shader_module->ubo_data_layouts.begin());
                if(it->type == UNIFORM_DATA_TYPE_BUFFER) {
                    buffer_infos.emplace_back();
                    buffer_infos.back().buffer = mp_ubo_allocator->GetUniformBuffer();
                    buffer_infos.back().offset = mp_ubo_allocator->GetAreaOffset(m_mod_id, id);
                    buffer_infos.back().range = it->ubo_size;
                }
            }

            buffer_infos.shrink_to_fit();
            return buffer_infos;
        }


        std::vector<VkDescriptorImageInfo> DescriptorSetsCreator::_FindImageInfos() {
            std::vector<VkDescriptorImageInfo> img_infos;
            img_infos.reserve(m_textures->size());

            for(auto it = m_textures->begin(); it != m_textures->end(); it++) {
                img_infos.emplace_back();
                img_infos.back().sampler = it->sampler;
                img_infos.back().imageView = it->image_view;
                img_infos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
            return img_infos;
        }


        std::vector<VkWriteDescriptorSet> DescriptorSetsCreator::_FindWriteDescriptorInfos(const std::vector<VkDescriptorBufferInfo> &_buf_infos, const std::vector<VkDescriptorImageInfo> &_img_infos) {
            std::vector<VkWriteDescriptorSet> write_sets;
            write_sets.reserve(m_descriptor_sets.size() * m_shader_module->ubo_data_layouts.size());

            uint32_t sc_lap = 0;
            uint32_t used_buffers = 0, used_imgs = 0;

            // for each ubo data layout fill write descriptor set struct
            for(auto desc_it = m_descriptor_sets.begin(); desc_it != m_descriptor_sets.end(); desc_it++) {
                for(auto ubo_it = m_shader_module->ubo_data_layouts.begin(); ubo_it != m_shader_module->ubo_data_layouts.end(); ubo_it++) {
                    write_sets.emplace_back();
                    write_sets.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write_sets.back().dstSet = *desc_it;
                    write_sets.back().dstBinding = ubo_it->binding;
                    write_sets.back().dstArrayElement = 0;
                    write_sets.back().descriptorCount = 1;

                    switch(ubo_it->type) {
                        case UNIFORM_DATA_TYPE_BUFFER:
                            write_sets.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            write_sets.back().pBufferInfo = &_buf_infos[used_buffers];

                            if((++sc_lap) % m_swapchain_images_count == 0)
                                used_buffers++;
                            break;

                        case UNIFORM_DATA_TYPE_IMAGE_SAMPLER:
                            write_sets.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            write_sets.back().pImageInfo = &_img_infos[used_imgs];

                            if((++sc_lap) % m_swapchain_images_count == 0)
                                used_imgs++;
                            break;

                        default:
                            DENG_ASSERT(false);
                            break;
                    }
                }
            }

            return write_sets;
        }
    }
}
