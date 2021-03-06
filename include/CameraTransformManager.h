// DENG: dynamic engine - small but powerful 2D and 3D game engine
// licence: Apache, see LICENCE file
// file: CameraTransformManager.h - camera transformation class header
// author: Karl-Mihkel Ott

/***** Deprecated ******/
#ifndef CAMERA_TRANSFORM_MANAGER_H
#define CAMERA_TRANSFORM_MANAGER_H


#ifdef CAMERA_TRANSFORM_MANAGER_CPP
    #include <cstdlib>
    #include <cstring>
    #include <cmath>
    #include <string>
#ifdef _DEBUG
    #include <iostream>
#endif

    #include <BaseTypes.h>
    #include <libdas/include/Vector.h>
    #include <libdas/include/Matrix.h>
    #include <libdas/include/Points.h>
    #include <libdas/include/Quaternion.h>
#endif


namespace DENG {

    class CameraTransformManager {
        private:
            // rotation quaternions relative to the camera system
            Libdas::Quaternion m_x_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Quaternion m_y_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Quaternion m_z_rot = Libdas::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

            // camera TR properties
            Libdas::Point3D<float> m_rotations = Libdas::Point3D<float>(0.0f, 0.0f, 0.0f);
            Libdas::Vector4<float> m_camera_pos = Libdas::Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Vector4<float> m_translation = Libdas::Vector4<float>(0.0f, 0.0f, 0.0f, 1.0f);
            Libdas::Vector3<float> m_translation_offset = Libdas::Vector3<float>(0.0f, 0.0f, 0.0f);
        private:
            void _PreventRotationOverflow();

        public:
            CameraTransformManager() = default;

            // NOTE: Relative means relative to camera coordinate set, absolute means relative to world coordinates
            void MoveCamera(const Libdas::Vector3<float> &_delta_mov, bool ignore_pitch_movement = false);
            void RotateCamera(const Libdas::Point3D<float> &_delta_rot);

            // z axis will be pointing to the given point
            void CustomPointOriginRotation(const Libdas::Point3D<float> &_delta_rot, const Libdas::Vector4<float> &_point);
            void CustomPointOriginMove(const Libdas::Vector3<float> &_delta_mov, const Libdas::Vector4<float> &_point);

            Libdas::Matrix4<float> ConstructViewMatrix();

            inline Libdas::Point3D<float> GetRotations() {
                return m_rotations;
            }

            inline Libdas::Vector4<float> GetPosition() {
                return m_camera_pos;
            }
    };
}

#endif
