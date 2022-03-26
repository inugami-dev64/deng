// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ModelShaderManager.cpp - DAS model shader management singleton class implementation
// author: Karl-Mihkel Ott

#define MODEL_SHADER_MANAGER_CPP
#include <ModelShaderManager.h>

namespace DENG {

    uint32_t ModelShaderManager::m_used_shaders = 0;
    std::array<uint32_t, 8> ModelShaderManager::m_shader_ids = std::array<uint32_t, 8>();
    bool ModelShaderManager::m_set_stage = false;

    uint32_t ModelShaderManager::_ResolveMask(uint32_t _mask) {
        DENG_ASSERT(_mask & POS_ATTR_MASK);

        if((_mask & UV_ATTR_MASK) && (_mask & NORMAL_ATTR_MASK) && (_mask & TANG_ATTR_MASK))
            return POS_UV_NORMAL_TANG;
        else if((_mask & UV_ATTR_MASK) && (_mask & NORMAL_ATTR_MASK))
            return POS_UV_NORMAL;
        else if((_mask & UV_ATTR_MASK) && (_mask & TANG_ATTR_MASK))
            return POS_UV_TANG;
        else if((_mask & NORMAL_ATTR_MASK) && (_mask & TANG_ATTR_MASK))
            return POS_NORMAL_TANG;
        else if((_mask & UV_ATTR_MASK))
            return POS_UV;
        else if((_mask & NORMAL_ATTR_MASK))
            return POS_NORMAL;
        else if((_mask & TANG_ATTR_MASK))
            return POS_TANG;

        return POS;
    }

    void ModelShaderManager::_GetAttributesByType(const Libdas::DasParser &_parser, const Libdas::DasMeshPrimitive &_prim, uint32_t _id, ShaderModule &_module, uint32_t _base_offset) {
        // pos
        _module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
        _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_buffer_id, _prim.vertex_buffer_offset));

        switch(_id) {
            case POS_UV_NORMAL_TANG:
                // uv
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.uv_buffer_id, _prim.uv_buffer_offset));
                // normal
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_normal_buffer_id, _prim.vertex_normal_buffer_offset));
                // tangent
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_tangent_buffer_id, _prim.vertex_tangent_buffer_offset));
                break;

            case POS_UV_NORMAL:
                // uv
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.uv_buffer_id, _prim.uv_buffer_offset));
                // normal
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_normal_buffer_id, _prim.vertex_normal_buffer_offset));
                break;

            case POS_UV_TANG:
                // uv
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.uv_buffer_id, _prim.uv_buffer_offset));
                // tangent
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_tangent_buffer_id, _prim.vertex_tangent_buffer_offset));
                break;

            case POS_NORMAL_TANG:
                // normal
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_normal_buffer_id, _prim.vertex_normal_buffer_offset));
                // tangent
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_tangent_buffer_id, _prim.vertex_tangent_buffer_offset));
                break;

            case POS_UV:
                // uv
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC2_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.uv_buffer_id, _prim.uv_buffer_offset));
                break;

            case POS_NORMAL:
                // normal
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC3_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_normal_buffer_id, _prim.vertex_normal_buffer_offset));
                break;

            case POS_TANG:
                // tangent
                _module.attributes.push_back(ATTRIBUTE_TYPE_VEC4_FLOAT);
                _module.offsets.push_back(_base_offset + MeshLoader::CalculateAbsoluteOffset(_parser, _prim.vertex_tangent_buffer_id, _prim.vertex_tangent_buffer_offset));
                break;

            default:
                break;
        }
    }


    void ModelShaderManager::_GetUniforms(Renderer &_rend, uint32_t _id, ShaderModule &_module, const uint32_t _camera_ubo_offset, uint32_t &_base_offset) {
        // there are 3 types of uniform objects in each shader module
        //   1. CameraUbo
        //   2. AnimationUbo
        //   3. ModelUbo

        // CameraUbo data
        _module.ubo_data_layouts.emplace_back();
        _module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        _module.ubo_data_layouts.back().binding = 1;
        _module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        _module.ubo_data_layouts.back().ubo_size = sizeof(ModelCameraUbo);
        _module.ubo_data_layouts.back().offset = _camera_ubo_offset;

        // AnimationUbo data
        _module.ubo_data_layouts.reserve(4);
        _module.ubo_data_layouts.emplace_back();
        _module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        _module.ubo_data_layouts.back().binding = 0;
        _module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        _module.ubo_data_layouts.back().ubo_size = sizeof(ModelAnimationUbo);
        _module.ubo_data_layouts.back().offset = _base_offset;
        _base_offset += static_cast<uint32_t>(sizeof(ModelAnimationUbo));

        // ModelUbo
        _module.ubo_data_layouts.emplace_back();
        _module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_BUFFER;
        _module.ubo_data_layouts.back().binding = 2;
        _module.ubo_data_layouts.back().stage = SHADER_STAGE_VERTEX;
        _module.ubo_data_layouts.back().ubo_size = sizeof(ModelUbo);
        _module.ubo_data_layouts.back().offset = _rend.AlignUniformBufferOffset(_base_offset);
        _base_offset += static_cast<uint32_t>(sizeof(ModelUbo));
            
        // check if uv attributes are present and if they are, add sampler
        if(_id == POS_UV_NORMAL_TANG || _id == POS_UV_NORMAL || _id == POS_UV_TANG || _id == POS_UV) {
            _module.ubo_data_layouts.emplace_back();
            _module.ubo_data_layouts.back().type = UNIFORM_DATA_TYPE_IMAGE_SAMPLER;
            _module.ubo_data_layouts.back().binding = 3;
            _module.ubo_data_layouts.back().stage = SHADER_STAGE_FRAGMENT;
        }
    }


    uint32_t ModelShaderManager::RequestShaderModule(Renderer &_rend, const Libdas::DasParser &_parser, const Libdas::DasMeshPrimitive &_mesh,
                                                     const uint32_t _base_offset, const uint32_t _camera_ubo_offset, uint32_t &_base_ubo_offset) {
        if(!m_set_stage) {
            memset(m_shader_ids.data(), 0xff, sizeof(uint32_t) * 8);
            m_set_stage = true;
        } 

        uint32_t mask = 0;
        if(_mesh.vertex_buffer_id != UINT32_MAX)
            mask |= POS_ATTR_MASK;
        if(_mesh.uv_buffer_id != UINT32_MAX)
            mask |= UV_ATTR_MASK;
        if(_mesh.vertex_normal_buffer_id != UINT32_MAX)
            mask |= NORMAL_ATTR_MASK;
        if(_mesh.vertex_tangent_buffer_id != UINT32_MAX)
            mask |= TANG_ATTR_MASK;

        uint32_t id = _ResolveMask(mask);

        if(m_shader_ids[id] == UINT32_MAX) {
            ShaderModule module;
            if(mask & UV_ATTR_MASK)
                module.fragment_shader_file = UV_FRAGMENT_SHADER;
            else module.fragment_shader_file = COLORED_SHADER;

            _GetAttributesByType(_parser, _mesh, id, module, _base_offset);
            _GetUniforms(_rend, id, module, _camera_ubo_offset, _base_ubo_offset);
            m_shader_ids[id] = _rend.PushShader(module);
        }

        return m_shader_ids[id];
    }
}