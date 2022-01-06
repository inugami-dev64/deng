// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: ShaderDefinitions.h - Cross api shader file and bindings definitions
// author: Karl-Mihkel Ott


#ifndef SHADER_DEFINITIONS_H
#define SHADER_DEFINITIONS_H

// Pipeline indices
#define UM2D_I          0
#define TM2D_I          1
#define UM3D_I          2
#define TM3D_I          3
#define UI_I            4
#define PIPELINE_C      5


/// Spir-V binary shaders
// UI system shaders
#define SPIR_V_UI_VERT_SHADER                       "./shaders/bin/2d/UIV.spv"
#define SPIR_V_UI_FRAG_SHADER                       "./shaders/bin/2d/UIF.spv"

// 2D asset shaders
#define SPIR_V_UNMAPPED_VERT_SHADER_2D              "./shaders/bin/2d/UV.spv"
#define SPIR_V_UNMAPPED_FRAG_SHADER_2D              "./shaders/bin/2d/UF.spv"
#define SPIR_V_TEXTURE_MAPPED_VERT_SHADER_2D        "./shaders/bin/2d/TMV.spv"
#define SPIR_V_TEXTURE_MAPPED_FRAG_SHADER_2D        "./shaders/bin/2d/TMF.spv"

// 3D asset shaders
#define SPIR_V_UNMAPPED_VERT_SHADER_3D              "./shaders/bin/3d/UV.spv"
#define SPIR_V_UNMAPPED_FRAG_SHADER_3D              "./shaders/bin/3d/UF.spv"
#define SPIR_V_TEXTURE_MAPPED_VERT_SHADER_3D        "./shaders/bin/3d/TMV.spv"
#define SPIR_V_TEXTURE_MAPPED_FRAG_SHADER_3D        "./shaders/bin/3d/TMF.spv"


/// Source GLSL shaders
// UI system shaders
#define SRC_UI_VERT_SHADER                          "./shaders/src/2d/imgui.vert"
#define SRC_UI_FRAG_SHADER                          "./shaders/src/2d/imgui.frag"

// 2D asset shaders
#define SRC_UNMAPPED_VERT_SHADER_2D                 "./shaders/src/2d/unmapped.vert"
#define SRC_UNMAPPED_FRAG_SHADER_2D                 "./shaders/src/2d/unmapped.frag"
#define SRC_TEXTURE_MAPPED_VERT_SHADER_2D           "./shaders/src/2d/tex_mapped.vert"
#define SRC_TEXTURE_MAPPED_FRAG_SHADER_2D           "./shaders/src/2d/tex_mapped.frag"

// 3D asset shaders
#define SRC_UNMAPPED_VERT_SHADER_3D                 "./shaders/src/3d/unmapped.vert"
#define SRC_UNMAPPED_FRAG_SHADER_3D                 "./shaders/src/3d/unmapped.frag"
#define SRC_TEXTURE_MAPPED_VERT_SHADER_3D           "./shaders/src/3d/tex_mapped.vert"
#define SRC_TEXTURE_MAPPED_FRAG_SHADER_3D           "./shaders/src/3d/tex_mapped.frag"

#endif