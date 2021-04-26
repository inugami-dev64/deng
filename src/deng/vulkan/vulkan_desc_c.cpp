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


#define __VULKAN_DESC_C_CPP
#include <deng/vulkan/vulkan_desc_c.h>

namespace deng {
    namespace vulkan {
        extern deng_ui32_t __max_frame_c;

        /****************************************************/
        /****************************************************/
        /********** __vk_DescriptorCreator class ************/
        /****************************************************/
        /****************************************************/

        __vk_DescriptorCreator::__vk_DescriptorCreator (
            VkDevice device, 
            VkExtent2D extent, 
            VkRenderPass renderpass, 
            deng::__GlobalRegistry &reg,
            std::vector<deng_Id> &assets,
            std::vector<deng_Id> &textures,
            VkSampleCountFlagBits sample_c
        ) : m_assets(assets), m_textures(textures), m_reg(reg) 
        {
            __mkDescriptorSetLayouts(device);   
            __mkPipelineLayouts(device);
            __mkGraphicsPipelines (
                device, 
                extent, 
                renderpass, 
                sample_c
            );
        }

        
        /*
         * Abstracted function for creating descriptor sets
         * for assets between the asset bounds
         */
        void __vk_DescriptorCreator::mkDS (
            VkDevice device,
            __vk_BufferData *p_bd,
            const char *dummy_tex_uuid,
            const dengMath::vec2<deng_ui32_t> &asset_bounds,
            deng_ui64_t min_ubo_align
        ) {
            // Count new assets by their fragment type
            for(size_t i = asset_bounds.first; i < asset_bounds.second; i++) {
                RegType &reg_vk_asset = m_reg.retrieve (
                    m_assets[i], 
                    DENG_SUPPORTED_REG_TYPE_VK_ASSET
                );

                RegType &reg_asset = m_reg.retrieve (
                    reg_vk_asset.vk_asset.base_id,
                    DENG_SUPPORTED_REG_TYPE_ASSET
                );

                if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED ||
                   reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED) 
                    m_mapped_asset_c++;

                else if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_UNMAPPED ||
                        reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) 
                    m_unmapped_asset_c++;
            }

            __checkDescPoolCapacity (
                device,
                p_bd, 
                dummy_tex_uuid,
                min_ubo_align
            );

            // Create new descriptor sets for all assets that are between given bounds
            for(size_t i = asset_bounds.first; i < asset_bounds.second; i++) {
                RegType &reg_vk_asset = m_reg.retrieve (
                    m_assets[i],
                    DENG_SUPPORTED_REG_TYPE_VK_ASSET
                );

                RegType &reg_asset = m_reg.retrieve (
                    reg_vk_asset.vk_asset.base_id,
                    DENG_SUPPORTED_REG_TYPE_ASSET
                );

                if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED ||
                   reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED) {
                    __mkTexMappedDS (
                        device,
                        reg_vk_asset.vk_asset,
                        p_bd,
                        dummy_tex_uuid,
                        min_ubo_align
                    );
                }

                else if(reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_UNMAPPED ||
                        reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED) {
                    __mkUnmappedDS (
                        device,
                        reg_vk_asset.vk_asset,
                        p_bd,
                        min_ubo_align
                    );
                }
            }
        }


        /* 
         * Create descriptor set layouts 
         */
        void __vk_DescriptorCreator::__mkDescriptorSetLayouts(VkDevice &device) {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            VkDescriptorSetLayoutCreateInfo layout_createinfo{};

            // Set up descriptor set layout bindings for uniform buffers
            VkDescriptorSetLayoutBinding ubo_matrix_binding{};
            ubo_matrix_binding.binding = 0;
            ubo_matrix_binding.descriptorCount = 1;
            ubo_matrix_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ubo_matrix_binding.pImmutableSamplers = NULL;
            ubo_matrix_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            // Set up descriptor set layout bindings for uniform color data
            VkDescriptorSetLayoutBinding ubo_color_binding{};
            ubo_color_binding.binding = 1;
            ubo_color_binding.descriptorCount = 1;
            ubo_color_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            ubo_color_binding.pImmutableSamplers = NULL;
            ubo_color_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            // Set up descriptor set layout bindings for image sampler
            VkDescriptorSetLayoutBinding sampler_binding{};
            sampler_binding.binding = 2;
            sampler_binding.descriptorCount = 1;
            sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            sampler_binding.pImmutableSamplers = NULL;
            sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;        
            
            // Use only uniform buffers for unmapped assets
            bindings.resize(2);
            bindings[0] = ubo_matrix_binding;
            bindings[1] = ubo_color_binding;

            // Set up descriptor set layout info for unmapped assets
            layout_createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layout_createinfo.bindingCount = static_cast<deng_ui32_t>(bindings.size());
            layout_createinfo.pBindings = bindings.data();
            if
            (
                vkCreateDescriptorSetLayout (
                    device, 
                    &layout_createinfo, 
                    NULL, 
                    &m_unmapped_desc_set_layout
                )
            ) VK_DESC_ERR("failed to create descriptor set layout for unmapped assets!");


            // Use add texture sampler bindings to descriptor set
            bindings.resize(3);
            bindings[0] = ubo_matrix_binding;
            bindings[1] = ubo_color_binding;
            bindings[2] = sampler_binding;

            VkDescriptorSetLayoutCreateInfo texture_mapped_layout_createinfo{};

            // Set up descriptor set layout info for texture mapped assets
            texture_mapped_layout_createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            texture_mapped_layout_createinfo.bindingCount = static_cast<deng_ui32_t>(bindings.size());
            texture_mapped_layout_createinfo.pBindings = bindings.data();

            // Create descriptor set layout for texture 
            if
            (
                vkCreateDescriptorSetLayout (
                    device, 
                    &texture_mapped_layout_createinfo, 
                    NULL, 
                    &m_tex_mapped_desc_set_layout
                ) != VK_SUCCESS
            ) VK_DESC_ERR("failed to create descriptor set layout for texture mapped assets!");
        }


        /* Create pipeline layouts */
        void __vk_DescriptorCreator::__mkPipelineLayouts(VkDevice &device) {
            // Set up pipeline layout info struct
            VkPipelineLayoutCreateInfo pipeline_layout_createinfo{};
            pipeline_layout_createinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_createinfo.setLayoutCount = 1;
            pipeline_layout_createinfo.pushConstantRangeCount = 0;
            pipeline_layout_createinfo.pSetLayouts = &m_unmapped_desc_set_layout;

            // Create pipeline layout for unmapped assets
            if
            (
                vkCreatePipelineLayout (
                    device, 
                    &pipeline_layout_createinfo, 
                    NULL, 
                    &m_unmapped_pl
                ) != VK_SUCCESS
            ) VK_DESC_ERR("failed to create pipeline layout for unmapped objects!");

            // Create pipeline layout for texture mapped assets
            pipeline_layout_createinfo.pSetLayouts = &m_tex_mapped_desc_set_layout;
            if
            (
                vkCreatePipelineLayout (
                    device, 
                    &pipeline_layout_createinfo, 
                    NULL, 
                    &m_tex_mapped_pl
                ) != VK_SUCCESS
            ) VK_DESC_ERR("failed to create pipeline layout for textured objects!");
        }


        /* 
         * Create graphics pipelines for different assets 
         */
        void __vk_DescriptorCreator::__mkGraphicsPipelines (
            VkDevice &device, 
            VkExtent2D &extent, 
            VkRenderPass &renderpass,
            VkSampleCountFlagBits sample_c
        ) {
            size_t index;
            
            // Specify the pipiline type and layout
            m_pipelines[UM3D_I].pipeline_type = DENG_PIPELINE_TYPE_UNMAPPED_3D;
            m_pipelines[TM3D_I].pipeline_type = DENG_PIPELINE_TYPE_TEXTURE_MAPPED_3D;
            m_pipelines[UM2D_I].pipeline_type = DENG_PIPELINE_TYPE_UNMAPPED_2D;
            m_pipelines[TM2D_I].pipeline_type = DENG_PIPELINE_TYPE_TEXTURE_MAPPED_2D;

            m_pipelines[UM3D_I].p_pipeline_layout = &m_unmapped_pl;
            m_pipelines[TM3D_I].p_pipeline_layout = &m_tex_mapped_pl;
            m_pipelines[UM2D_I].p_pipeline_layout = &m_unmapped_pl;
            m_pipelines[TM2D_I].p_pipeline_layout = &m_tex_mapped_pl;

            __vk_PipelineCreator unmapped_pipeline_3d (
                &m_pipelines[UM3D_I],
                device, 
                extent, 
                renderpass
            );

            __vk_PipelineCreator texture_mapped_pipeline_3d (
                &m_pipelines[TM3D_I], 
                device, 
                extent, 
                renderpass
            );

            __vk_PipelineCreator unmapped_pipeline_2d (
                &m_pipelines[UM2D_I], 
                device, 
                extent, 
                renderpass
            );

            __vk_PipelineCreator texture_mapped_pipeline_2d (
                &m_pipelines[TM2D_I], 
                device, 
                extent, 
                renderpass
            );

            std::array<VkGraphicsPipelineCreateInfo, DENG_PIPELINE_COUNT> pipeline_infos{};
            pipeline_infos[UM3D_I] = unmapped_pipeline_3d.mkGraphicsPipelineInfo (
                UNMAPPED_VERT_SHADER_3D, 
                UNMAPPED_FRAG_SHADER, 
                (char*) "main", 
                VK_POLYGON_MODE_FILL, 
                VK_CULL_MODE_NONE, 
                VK_FRONT_FACE_COUNTER_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                true, 
                false, 
                sample_c,
                0
            );

            pipeline_infos[TM3D_I] = texture_mapped_pipeline_3d.mkGraphicsPipelineInfo (
                TEXTURE_MAPPED_VERT_SHADER_3D, 
                TEXTURE_MAPPED_FRAG_SHADER, 
                (char*) "main", 
                VK_POLYGON_MODE_FILL, 
                VK_CULL_MODE_NONE, 
                VK_FRONT_FACE_COUNTER_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                true, 
                false,
                sample_c, 
                0
            );

            pipeline_infos[UM2D_I] = unmapped_pipeline_2d.mkGraphicsPipelineInfo (
                UNMAPPED_VERT_SHADER_2D, 
                UNMAPPED_FRAG_SHADER, 
                (char*) "main", 
                VK_POLYGON_MODE_FILL, 
                VK_CULL_MODE_NONE, 
                VK_FRONT_FACE_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                false, 
                false, 
                sample_c,
                0
            );

            pipeline_infos[TM2D_I] = texture_mapped_pipeline_2d.mkGraphicsPipelineInfo (
                TEXTURE_MAPPED_VERT_SHADER_2D, 
                TEXTURE_MAPPED_FRAG_SHADER, 
                (char*) "main", 
                VK_POLYGON_MODE_FILL, 
                VK_CULL_MODE_NONE, 
                VK_FRONT_FACE_CLOCKWISE, 
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
                true, 
                false, 
                sample_c,
                0
            );

            std::array<VkPipeline, DENG_PIPELINE_COUNT> pipelines;
            if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, static_cast<deng_ui32_t>(pipeline_infos.size()), 
               pipeline_infos.data(), NULL, pipelines.data()) != VK_SUCCESS) 
                VK_DESC_ERR("failed to create graphics pipelines!");

            else {
                for(index = 0; index < pipelines.size(); index++)
                    m_pipelines[index].pipeline = pipelines[index];
            }
        }


        /* 
         * Create descriptor pool for descriptor sets without texture sampling 
         */
        void __vk_DescriptorCreator::__mkUnmappedDescPool (
            VkDevice device,
            deng_ui32_t desc_c
        ) {
            LOG("Creating descriptor pool for unmapped vertices");
            VkDescriptorPoolSize desc_pool_size;
            VkDescriptorPoolCreateInfo desc_pool_createinfo{};

            desc_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_size.descriptorCount = desc_c * __max_frame_c;

            // Set up descriptor pool createinfo 
            desc_pool_createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_createinfo.poolSizeCount = 1;
            desc_pool_createinfo.pPoolSizes = &desc_pool_size;
            desc_pool_createinfo.maxSets = desc_c * __max_frame_c;

            // Create descriptor pool for unmapped assets 
            if(vkCreateDescriptorPool(device, &desc_pool_createinfo, NULL, &m_unmapped_desc_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool for unmapped assets");
        }


        
        /* Create descriptor pool for descriptor sets without texture sampling */
        void __vk_DescriptorCreator::__mkTexMappedDescPool (
            VkDevice device,
            deng_ui32_t desc_c
        ) {
            LOG("Creating descriptor pool for texture mapped vertices");
            std::array<VkDescriptorPoolSize, 3> desc_pool_sizes;
            VkDescriptorPoolCreateInfo desc_pool_createinfo{};
            m_tex_cap += (deng_ui32_t) (2 * m_textures.size());

            desc_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[0].descriptorCount = m_tex_cap * __max_frame_c;
            desc_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc_pool_sizes[1].descriptorCount = m_tex_cap * __max_frame_c;
            desc_pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc_pool_sizes[2].descriptorCount = m_tex_cap * __max_frame_c;

            desc_pool_createinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            desc_pool_createinfo.poolSizeCount = (deng_ui32_t) desc_pool_sizes.size();
            desc_pool_createinfo.pPoolSizes = desc_pool_sizes.data();
            desc_pool_createinfo.maxSets = m_tex_cap * __max_frame_c;
            desc_pool_createinfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            
            if(vkCreateDescriptorPool(device, &desc_pool_createinfo, NULL, &m_tex_mapped_desc_pool) != VK_SUCCESS)
                VK_DESC_ERR("failed to create descriptor pool for texture mapped assets");
        }


        /* 
         * Create unmapped descriptor sets for single asset
         */
        void __vk_DescriptorCreator::__mkUnmappedDS (
            VkDevice device, 
            __vk_Asset &asset,
            __vk_BufferData *p_bd,
            deng_ui64_t min_align
        ) {
            size_t index;

            // Set up multiple descriptor layout structures
            std::vector<VkDescriptorSetLayout> tmp_descriptor_set_layouts;
            tmp_descriptor_set_layouts.resize(__max_frame_c);

            std::array<VkDescriptorBufferInfo, 2> bufferinfos;
            std::array<VkWriteDescriptorSet, 2> descriptor_writes{};
            VkDescriptorSetAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

            // Allocate memory for descriptor sets
            asset.desc_c = __max_frame_c;
            asset.desc_sets = (VkDescriptorSet*) calloc (
                asset.desc_c,
                sizeof(VkDescriptorSet)
            );

            for(index = 0; index < tmp_descriptor_set_layouts.size(); index++) 
                tmp_descriptor_set_layouts[index] = m_unmapped_desc_set_layout;
            
            allocinfo.descriptorPool = m_unmapped_desc_pool;
            allocinfo.descriptorSetCount = (deng_ui32_t) tmp_descriptor_set_layouts.size();
            allocinfo.pSetLayouts = tmp_descriptor_set_layouts.data();

            /* Allocate descriptor sets for unmapped 3D vertices */
            if(vkAllocateDescriptorSets(device, &allocinfo, asset.desc_sets) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets for unmapped assets");

            for(index = 0; index < __max_frame_c; index++) {
                // Set up descriptor buffer info struct
                bufferinfos[0].buffer = p_bd->uniform_buffer;
                bufferinfos[0].offset = index * std::max<deng_ui64_t>(sizeof(__vk_UniformColorData), min_align);
                bufferinfos[0].range = sizeof(__vk_UniformTransformation);

                bufferinfos[1].buffer = p_bd->uniform_buffer;
                bufferinfos[1].offset = asset.color_offset + index * std::max<deng_ui64_t>(sizeof(__vk_UniformColorData), min_align);
                bufferinfos[1].range = sizeof(__vk_UniformColorData);

                // Set up descriptor set write
                descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_writes[0].dstSet = asset.desc_sets[index];
                descriptor_writes[0].dstBinding = 0;
                descriptor_writes[0].dstArrayElement = 0;
                descriptor_writes[0].descriptorCount = 1;
                descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptor_writes[0].pBufferInfo = &bufferinfos[0];
        
                descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_writes[1].dstSet = asset.desc_sets[index];
                descriptor_writes[1].dstBinding = 1;
                descriptor_writes[1].dstArrayElement = 0;
                descriptor_writes[1].descriptorCount = 1;
                descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptor_writes[1].pBufferInfo = &bufferinfos[1];


                // Update descriptor sets
                vkUpdateDescriptorSets (
                    device, 
                    (deng_ui32_t) descriptor_writes.size(), 
                    descriptor_writes.data(), 
                    0, 
                    NULL
                );
            }
        }
        

        /* 
         * Create texture mapped descriptor sets for one asset
         */
        void __vk_DescriptorCreator::__mkTexMappedDS (
            VkDevice device, 
            __vk_Asset &asset,
            __vk_BufferData *p_bd,
            const char *dummy_tex_uuid,
            deng_ui64_t min_align
        ) {
            size_t i;

            // Create temporary descriptor set layouts
            std::vector<VkDescriptorSetLayout> tmp_descriptor_set_layouts;
            tmp_descriptor_set_layouts.resize(__max_frame_c);

            // Set up information structs for allocating descriptor sets
            std::array<VkDescriptorBufferInfo, 2> bufferinfo;
            std::array<VkWriteDescriptorSet, 3> descriptor_writes{};
            VkDescriptorImageInfo desc_imageinfo{};
            VkDescriptorSetAllocateInfo allocinfo{};
            allocinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

            for(i = 0; i < tmp_descriptor_set_layouts.size(); i++)
                tmp_descriptor_set_layouts[i] = m_tex_mapped_desc_set_layout;

            // Fill out VkDescriptorSetAllocateInfo instance
            asset.desc_c = __max_frame_c;
            asset.desc_sets = (VkDescriptorSet*) calloc (
                asset.desc_c,
                sizeof(VkDescriptorSet)
            );

            allocinfo.descriptorPool = m_tex_mapped_desc_pool;
            allocinfo.descriptorSetCount = static_cast<deng_ui32_t>(tmp_descriptor_set_layouts.size());
            allocinfo.pSetLayouts = tmp_descriptor_set_layouts.data();
            
            // Allocate descriptor sets
            if(vkAllocateDescriptorSets(device, &allocinfo, asset.desc_sets) != VK_SUCCESS)
                VK_DESC_ERR("failed to allocate descriptor sets for texture mapped assets");

            // Iterate through every descriptor set in swapchain image and update it
            for(i = 0; i < __max_frame_c; i++) {
                // Set up uniform transformation data buffer
                bufferinfo[0].buffer = p_bd->uniform_buffer;
                bufferinfo[0].offset = i * std::max<VkDeviceSize>(sizeof(__vk_UniformTransformation), min_align);
                bufferinfo[0].range = sizeof(__vk_UniformTransformation);

                // Set up uniform color data buffer for each texture instance
                bufferinfo[1].buffer = p_bd->uniform_buffer;
                bufferinfo[1].offset = asset.color_offset + i * std::max<VkDeviceSize>(sizeof(__vk_UniformColorData), min_align);
                bufferinfo[1].range = sizeof(__vk_UniformColorData);

                // Check if the requested texture is present otherwise attach dummy texture
                RegType *p_reg_tex = NULL;
                RegType &reg_asset = m_reg.retrieve (
                    asset.base_id, 
                    DENG_SUPPORTED_REG_TYPE_ASSET
                );

                if(reg_asset.asset.tex_uuid) {
                    RegType reg_tex = m_reg.retrieve (
                        reg_asset.asset.tex_uuid, 
                        DENG_SUPPORTED_REG_TYPE_TEXTURE
                    );

                    p_reg_tex = m_reg.retrievePtr (
                        reg_tex.tex.vk_id,
                        DENG_SUPPORTED_REG_TYPE_VK_TEXTURE
                    );
                }

                else {
                    p_reg_tex = m_reg.retrievePtr (
                        (deng_Id) dummy_tex_uuid,
                        DENG_SUPPORTED_REG_TYPE_VK_TEXTURE
                    );
                }

                LOG("Binded texture base id: " + std::string(p_reg_tex->vk_tex.uuid));
                desc_imageinfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                desc_imageinfo.imageView = p_reg_tex->vk_tex.image_view;
                desc_imageinfo.sampler = p_reg_tex->vk_tex.sampler;

                // Set up descriptor writes structs for texture mapped assets
                descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_writes[0].dstSet = asset.desc_sets[i];
                descriptor_writes[0].dstBinding = 0;
                descriptor_writes[0].dstArrayElement = 0;
                descriptor_writes[0].descriptorCount = 1;
                descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptor_writes[0].pBufferInfo = &bufferinfo[0];

                descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_writes[1].dstSet = asset.desc_sets[i];
                descriptor_writes[1].dstBinding = 1;
                descriptor_writes[1].dstArrayElement = 0;
                descriptor_writes[1].descriptorCount = 1;
                descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptor_writes[1].pBufferInfo = &bufferinfo[1];

                descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptor_writes[2].dstSet = asset.desc_sets[i];
                descriptor_writes[2].dstBinding = 2;
                descriptor_writes[2].dstArrayElement = 0;
                descriptor_writes[2].descriptorCount = 1;
                descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptor_writes[2].pImageInfo = &desc_imageinfo;

                // Update texture mapped descriptor sets
                vkUpdateDescriptorSets (
                    device, 
                    (deng_ui32_t) descriptor_writes.size(), 
                    descriptor_writes.data(), 
                    0, 
                    NULL
                );
            }
        }


        /*
         * Check if descriptor pools capacity is large enough
         * to allocate more descriptor sets there.
         * If descriptor pool is not large enough, all allocated descriptor
         * sets will be destroyed and new ones allocated in the new descriptor
         * pool
         */
        void __vk_DescriptorCreator::__checkDescPoolCapacity (
            VkDevice device,
            __vk_BufferData *p_bd,
            const char *dummy_tex_uuid,
            deng_ui64_t min_ubo_align
        ) {
            // Check if new unmapped descriptor pool need to be allocated
            if(!m_unmapped_asset_c) {
                m_unmapped_dc_pool_cap = __default_unmapped_ds_cap < m_unmapped_asset_c ?
                    m_unmapped_asset_c * 2 :
                    __default_unmapped_ds_cap;

                __mkUnmappedDescPool (
                    device, 
                    m_unmapped_dc_pool_cap
                );
            }

            else if(m_unmapped_asset_c > m_unmapped_dc_pool_cap) {
                m_unmapped_dc_pool_cap = m_unmapped_asset_c * 2;
                const std::vector<size_t> &inds = __cleanUnmappedDescSets(device);
                vkDestroyDescriptorPool (
                    device,
                    m_unmapped_desc_pool,
                    NULL
                );

                __mkUnmappedDescPool (
                    device,
                    m_unmapped_dc_pool_cap
                );

                for(size_t i = 0; i < inds.size(); i++) {
                    RegType &reg_asset = m_reg.retrieve (
                        m_assets[inds[i]], 
                        DENG_SUPPORTED_REG_TYPE_VK_ASSET
                    );

                    __mkUnmappedDS (
                        device, 
                        reg_asset.vk_asset,
                        p_bd,
                        min_ubo_align
                    );
                }
            }
            
            // Check if new texture mapped descriptor pool needs to be allocated
            if(!m_mapped_dc_pool_cap) {
                m_mapped_dc_pool_cap = __default_mapped_ds_cap < m_mapped_asset_c ?
                    m_mapped_asset_c * 2 :
                    __default_mapped_ds_cap;

                __mkTexMappedDescPool (
                    device, 
                    m_mapped_dc_pool_cap
                );
            }

            else if(m_mapped_asset_c > m_mapped_dc_pool_cap) {
                m_mapped_dc_pool_cap = m_mapped_asset_c * 2;
                const std::vector<size_t> &inds =__cleanUnmappedDescSets(device);
                vkDestroyDescriptorPool (
                    device,
                    m_tex_mapped_desc_pool,
                    NULL
                );

                __mkTexMappedDescPool (
                    device,
                    m_mapped_dc_pool_cap
                );

                // Create new descriptor sets for destroyed ones
                for(size_t i = 0; i < inds.size(); i++) {
                    RegType &vk_reg_asset = m_reg.retrieve (
                        m_assets[inds[i]], 
                        DENG_SUPPORTED_REG_TYPE_VK_ASSET
                    );

                    __mkTexMappedDS (
                        device,
                        vk_reg_asset.vk_asset,
                        p_bd,
                        dummy_tex_uuid,
                        min_ubo_align
                    );
                }
            }
        }


        /*
         * Destroy all texture mapped descriptor sets from assets
         * Return value is a vector with all indices to assets that
         * had their descriptor sets freed
         */
        const std::vector<size_t> &__vk_DescriptorCreator::__cleanMappedDescSets(VkDevice device) {
            static std::vector<size_t> out_inds;
            out_inds.reserve(m_assets.size());
            for(size_t i = 0; i < m_assets.size(); i++) {
                RegType &vk_reg_asset = m_reg.retrieve (
                    m_assets[i], 
                    DENG_SUPPORTED_REG_TYPE_VK_ASSET
                );
                
                RegType &reg_asset = m_reg.retrieve (
                    vk_reg_asset.vk_asset.base_id,
                    DENG_SUPPORTED_REG_TYPE_ASSET
                );

                if(reg_asset.vk_asset.is_desc && 
                  (reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_TEXTURE_MAPPED || 
                   reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_TEXTURE_MAPPED)) {
                    out_inds.resize(out_inds.size() + 1);
                    out_inds[out_inds.size() - 1] = i;
                    vkFreeDescriptorSets (
                        device, 
                        m_tex_mapped_desc_pool, 
                        vk_reg_asset.vk_asset.desc_c, 
                        vk_reg_asset.vk_asset.desc_sets
                    );
                }
            }

            return out_inds;
        }

        
        /*
         * Destroy all unmapped descriptor sets from assets
         * Return value is a vector with all indices to
         * the unmapped cleaned assets
         */
        const std::vector<size_t> &__vk_DescriptorCreator::__cleanUnmappedDescSets(VkDevice device) {
            static std::vector<size_t> out_inds;
            out_inds.reserve(m_assets.size());
            for(size_t i = 0; i < m_assets.size(); i++) {
                RegType &vk_reg_asset = m_reg.retrieve (
                    m_assets[i], 
                    DENG_SUPPORTED_REG_TYPE_VK_ASSET
                );
                
                RegType &reg_asset = m_reg.retrieve (
                    vk_reg_asset.vk_asset.base_id,
                    DENG_SUPPORTED_REG_TYPE_ASSET
                );

                if(vk_reg_asset.vk_asset.is_desc && 
                  (reg_asset.asset.asset_mode == DAS_ASSET_MODE_2D_UNMAPPED ||
                   reg_asset.asset.asset_mode == DAS_ASSET_MODE_3D_UNMAPPED)) {
                    out_inds.resize(out_inds.size() + 1);
                    out_inds[out_inds.size() - 1] = i;
                    vkFreeDescriptorSets (
                        device, 
                        m_unmapped_desc_pool, 
                        vk_reg_asset.vk_asset.desc_c, 
                        vk_reg_asset.vk_asset.desc_sets
                    );
                }
            }

            return out_inds;
        }


        /* __vk_DescriptorCreator getters */
        std::array<__vk_PipelineData, DENG_PIPELINE_COUNT> &__vk_DescriptorCreator::getPipelines() { return m_pipelines; }
        VkPipelineLayout __vk_DescriptorCreator::getUnmappedPL() { return m_unmapped_pl; }
        VkPipelineLayout __vk_DescriptorCreator::getTexMappedPL() { return m_tex_mapped_pl; }
        VkDescriptorSetLayout __vk_DescriptorCreator::getUnmappedDSL() { return m_unmapped_desc_set_layout; }
        VkDescriptorSetLayout __vk_DescriptorCreator::getTexMappedDSL() { return m_tex_mapped_desc_set_layout; }
        VkDescriptorPool __vk_DescriptorCreator::getUnmappedDP() { return m_unmapped_desc_pool; }
        VkDescriptorPool __vk_DescriptorCreator::getTexMappedDP() { return m_tex_mapped_desc_pool; }
    }
}
