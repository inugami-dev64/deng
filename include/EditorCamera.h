// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: EditorCamera.h - Editor camera class header
// author: Karl-Mihkel Ott


#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H


#ifdef EDITOR_CAMERA_CPP
    #include <string>
    #include <cstring>
    #include <vector>
    #include <variant>
    #include <chrono>
    #include <cmath>
    #include <unordered_map>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <libdas/include/Points.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Quaternion.h>

    #include <Api.h>
    #include <BaseTypes.h>
    #include <ErrorDefinitions.h>
    #include <ShaderDefinitions.h>
    #include <Window.h>
    #include <Renderer.h>
    #include <ModelUniforms.h>
    #include <Camera3D.h>
#endif

namespace DENG {

    class DENG_API EditorCamera : public Camera3D {
        private:
            bool m_is_enabled = false;
            Libdas::Point3D<float> m_origin = { 0.0f, 0.0f, 0.0f };

            // camera TR properties
            Libdas::Point3D<float> m_translation = { 0.0f, 0.0f, 0.0f };
            Libdas::Point3D<float> m_rotation = { 0.0f, 0.0f, 0.0f };

        private:
            void _ForceLimits();
            void _ConstructViewMatrix();

        public:
            EditorCamera(Renderer &_rend, Window &_win, const Camera3DConfiguration &_conf, const std::string &_name);

            virtual void EnableCamera() override;
            virtual void DisableCamera() override;
            virtual void Update() override;
    };
}

#endif
