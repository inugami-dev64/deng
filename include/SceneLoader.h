// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: SceneLoader.h - Scene loader class header
// author: Karl-Mihkel Ott

#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#ifdef SCENE_LOADER_CPP
    #include <vector>
    #include <string>
    #include <fstream>
    #include <chrono>
    #include <unordered_map>
    #include <any>
    #include <cmath>
    #include <cstring>
    #include <algorithm>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>
    #include <libdas/include/Api.h>
    #include <libdas/include/ErrorHandlers.h>
    #include <libdas/include/AsciiStreamReader.h>
    #include <libdas/include/AsciiLineReader.h>
    #include <libdas/include/DasStructures.h>
    #include <libdas/include/DasReaderCore.h>
    #include <libdas/include/DasParser.h>

    #include <Api.h>
    #include <ShaderDefinitions.h>
    #include <ErrorDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <ModelUniforms.h>
    #include <MeshLoader.h>
    #include <AnimationSampler.h>
    #include <SkeletonLoader.h>
    #include <NodeLoader.h>
#endif

namespace DENG {

    class SceneLoader {
        friend class ModelLoader;
        private:
            static uint32_t m_scene_index;

        private:
            std::vector<NodeLoader> m_root_node_loaders;
            std::string m_scene_name = "Unnamed scene";

        private:
            // recursive parser setting method
            // meant for ModelLoader classes move constructor
            void _SetParser(Libdas::DasParser &_parser);

        public:
            SceneLoader(
                Renderer &_rend, 
                Libdas::DasParser &_parser, 
                const Libdas::DasScene &_scene, 
                const std::vector<uint32_t> &_main_buffer_offsets, 
                uint32_t _camera_offset, 
                std::vector<Animation> &_animations,
                const std::string &_framebuffer_id
            );
            SceneLoader(const SceneLoader &_sl) = delete;
            SceneLoader(SceneLoader&& _sl) noexcept;
            void Update();

            inline const std::string &GetName() const {
                return m_scene_name;
            }

            inline std::vector<NodeLoader> &GetRootNodes() {
                return m_root_node_loaders;
            }
    };
}

#endif
