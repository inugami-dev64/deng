// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderDefinitions.h - Shader module definitions implementation
// author: Karl-Mihkel Ott

#define SHADER_DEFINITIONS_CPP
#include <ShaderDefinitions.h>

namespace DENG {

    std::size_t CalculateStride(const ShaderModule &_module) {
        std::size_t stride = 0;
        for(auto it = _module.attributes.begin(); it != _module.attributes.end(); it++) {
            switch(*it) {
                case ATTRIBUTE_TYPE_UBYTE:
                case ATTRIBUTE_TYPE_BYTE:
                    stride++;
                    break;

                case ATTRIBUTE_TYPE_SHORT:
                case ATTRIBUTE_TYPE_USHORT:
                case ATTRIBUTE_TYPE_VEC2_BYTE:
                case ATTRIBUTE_TYPE_VEC2_UBYTE:
                    stride += 2;
                    break;

                case ATTRIBUTE_TYPE_VEC3_BYTE:
                case ATTRIBUTE_TYPE_VEC3_UBYTE:
                    stride += 3;
                    break;

                case ATTRIBUTE_TYPE_FLOAT:
                case ATTRIBUTE_TYPE_UINT:
                case ATTRIBUTE_TYPE_INT:
                case ATTRIBUTE_TYPE_VEC2_SHORT:
                case ATTRIBUTE_TYPE_VEC2_USHORT:
                case ATTRIBUTE_TYPE_VEC4_BYTE:
                case ATTRIBUTE_TYPE_VEC4_UBYTE:
                    stride += 4;
                    break;

                case ATTRIBUTE_TYPE_VEC3_SHORT:
                case ATTRIBUTE_TYPE_VEC3_USHORT:
                    stride += 6;
                    break;

                case ATTRIBUTE_TYPE_DOUBLE:
                case ATTRIBUTE_TYPE_VEC2_FLOAT:
                case ATTRIBUTE_TYPE_VEC2_UINT:
                case ATTRIBUTE_TYPE_VEC2_INT:
                case ATTRIBUTE_TYPE_VEC4_SHORT:
                case ATTRIBUTE_TYPE_VEC4_USHORT:
                    stride += 8;
                    break;

                case ATTRIBUTE_TYPE_VEC3_FLOAT:
                case ATTRIBUTE_TYPE_VEC3_UINT:
                case ATTRIBUTE_TYPE_VEC3_INT:
                    stride += 12;
                    break;

                case ATTRIBUTE_TYPE_VEC4_FLOAT:
                case ATTRIBUTE_TYPE_VEC4_UINT:
                case ATTRIBUTE_TYPE_VEC4_INT:
                case ATTRIBUTE_TYPE_VEC2_DOUBLE:
                    stride += 16;
                    break;

                case ATTRIBUTE_TYPE_VEC3_DOUBLE:
                    stride += 24;
                    break;

                case ATTRIBUTE_TYPE_VEC4_DOUBLE:
                    stride += 32;
                    break;

                default:
                    DENG_ASSERT(false);
                    break;
            }
        }

        return stride;
    }


}