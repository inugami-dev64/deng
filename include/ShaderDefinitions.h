// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderDefinitions.h - Shader module definitions header
// author: Karl-Mihkel Ott


#ifndef SHADER_DEFINITIONS_H
#define SHADER_DEFINITIONS_H

#ifdef SHADER_DEFINITIONS_CPP
    #include <cstdint>
    #include <vector>
    #include <string>
#endif


namespace DENG {
    enum AttributeType {
        // single element attribute
        ATTRIBUTE_TYPE_FLOAT,
        ATTRIBUTE_TYPE_DOUBLE,
        ATTRIBUTE_TYPE_SHORT,
        ATTRIBUTE_TYPE_UINT,
        ATTRIBUTE_TYPE_INT,

        // two element attribute
        ATTRIBUTE_TYPE_VEC2_FLOAT,
        ATTRIBUTE_TYPE_VEC2_DOUBLE,
        ATTRIBUTE_TYPE_VEC2_SHORT,
        ATTRIBUTE_TYPE_VEC2_UINT,
        ATTRIBUTE_TYPE_VEC2_INT,

        // three element attribute
        ATTRIBUTE_TYPE_VEC3_FLOAT,
        ATTRIBUTE_TYPE_VEC3_DOUBLE,
        ATTRIBUTE_TYPE_VEC3_SHORT,
        ATTRIBUTE_TYPE_VEC3_UINT,
        ATTRIBUTE_TYPE_VEC3_INT,

        // four element attribute
        ATTRIBUTE_TYPE_VEC4_FLOAT,
        ATTRIBUTE_TYPE_VEC4_DOUBLE,
        ATTRIBUTE_TYPE_VEC4_SHORT,
        ATTRIBUTE_TYPE_VEC4_UINT,
        ATTRIBUTE_TYPE_VEC4_INT
    };


    enum UniformDataType {
        UNIFORM_DATA_TYPE_BUFFER,
        UNIFORM_DATA_TYPE_IMAGE_SAMPLER
    };


    typedef uint8_t ShaderStage;
    #define SHADER_STAGE_VERTEX     0x01
    #define SHADER_STAGE_GEOMETRY   0x02
    #define SHADER_STAGE_FRAGMENT   0x04

    // Structure to be passed into the renderer
    struct UniformDataLayout {
        UniformDataType type;
        uint32_t binding = 0;
        ShaderStage stage;
        uint32_t ubo_size;
        uint32_t offset = UINT32_MAX; // reserved value meaning no offset is bound
    };


    // this structure defines shaders that should be used in a pipeline
    // NOTE: geometry shaders are completely optional and not required
    struct ShaderModule {
        std::vector<AttributeType> attributes;
        std::vector<std::size_t> offsets;
        std::vector<UniformDataLayout> ubo_data_layouts;
        std::string vertex_shader_file = "";
        std::string geometry_shader_file = "";
        std::string fragment_shader_file = "";
        std::string vertex_shader_src;
        std::string geometry_shader_src;
        std::string fragment_shader_src;
        bool load_shaders_from_file = false;
    };

    std::size_t CalculateStride(ShaderModule const *_p_module);
}

#endif
