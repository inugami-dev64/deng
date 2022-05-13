#define SKELETON_DATA_MANAGER_CPP
#include <SkeletonDataManager.h>

namespace DENG {

    uint32_t SkeletonDataManager::m_skeleton_index = 0;

    SkeletonDataManager::SkeletonDataManager(const Libdas::Matrix4<float> &_node, Libdas::DasParser &_parser, const Libdas::DasSkeleton &_skeleton, std::vector<Animation> &_animation_sampler) : 
        m_parser(_parser),
        m_skeleton(_skeleton)
    {
        m_joint_matrices.resize(m_skeleton.joint_count);

        // find the maximum joint id
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
            if(m_skeleton.joints[i] > m_max_joint)
                m_max_joint = m_skeleton.joints[i];
        }

        m_max_joint++;
        m_joint_lookup.resize(m_max_joint);

        // fill lookup table
        for(uint32_t i = 0; i < m_skeleton.joint_count; i++)
            m_joint_lookup[m_skeleton.joints[i]] = i;

        _ApplyJointTransforms(_node, Libdas::Matrix4<float>(), m_skeleton.parent);

        // check which animation sampler joint ids are used in current skeleton
        for(auto ani_it = _animation_sampler.begin(); ani_it != _animation_sampler.end(); ani_it++) {
            for(auto smp_it = ani_it->second.begin(); smp_it != ani_it->second.end(); smp_it++) {
                for(uint32_t i = 0; i < m_skeleton.joint_count; i++) {
                    if(m_skeleton.joints[i] == smp_it->GetAnimationChannel().joint_id)
                        m_joint_samplers.push_back(&(*smp_it));
                }
            }
        }

        if(m_skeleton.name != "")
            m_skeleton_name = m_skeleton.name;
        else m_skeleton_name += std::to_string(m_skeleton_index++);
    }


    void SkeletonDataManager::_ApplyJointTransforms(const Libdas::Matrix4<float> &_parent, const Libdas::Matrix4<float> &_custom, uint32_t _joint_id) {
        const Libdas::DasSkeletonJoint &joint = m_parser.AccessSkeletonJoint(m_joint_lookup[_joint_id]);
        const Libdas::Matrix4<float> t = Libdas::Matrix4<float> {
            { 1.0f, 0.0f, 0.0f, joint.translation.x },
            { 0.0f, 1.0f, 0.0f, joint.translation.y },
            { 0.0f, 0.0f, 1.0f, joint.translation.z },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        const Libdas::Matrix4<float> r = joint.rotation.ExpandToMatrix4();
        const Libdas::Matrix4<float> s = Libdas::Matrix4<float> {
            { joint.scale, 0.0f, 0.0f, 0.0f },
            { 0.0f, joint.scale, 0.0f, 0.0f },
            { 0.0f, 0.0f, joint.scale, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f },
        };

        m_joint_matrices[m_joint_lookup[_joint_id]] = (s * r * t) * _custom * _parent * joint.inverse_bind_pos.Transpose();

        for(uint32_t i = 0; i < joint.children_count; i++) {
            _ApplyJointTransforms(m_joint_matrices[m_joint_lookup[_joint_id]], _custom, joint.children[i]);
        }
    }


    void SkeletonDataManager::Update() {
        // update animation samplers
        for(auto it = m_joint_samplers.begin(); it != m_joint_samplers.end(); it++) {
            (*it)->Update();
            auto ani_mat = (*it)->GetAnimationMatrix();
            //_ApplyJointTransforms(m_joint_matrices[(*it)->GetAnimationChannel().joint_id], ani_mat, (*it)->GetAnimationChannel().joint_id);
        }

        for(auto it = m_joint_matrices.begin(); it != m_joint_matrices.end(); it++)
            *it = it->Transpose();
    }
}
