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


#define __IMGUI_SANDBOX_CPP
#include <imgui-layer/imgui_sandbox.h>

namespace deng {

    ImGUIApplication::ImGUIApplication() : 
        m_win(1280, 720, "ImGUI demo"),
        m_cam(DENG_CAMERA_TYPE_EDITOR, static_cast<deng_vec_t>(dengMath::Conversion::degToRad(65.0)), 
            {0.1f, -25.0f}, {0.7f, 0.7f, 0.7f}, {0.3f, 0.3f}, false, &m_win),
        m_rend(DENG_RENDERER_HINT_API_VULKAN | DENG_RENDERER_HINT_ENABLE_API_DEBUGGING | DENG_RENDERER_HINT_MSAA_2, 
            dengMath::vec4<deng_vec_t>{0.0f, 0.0f, 0.0f, 1.0f})
    { 
        // Set bindings
        deng::Camera3DBindings bindings = {};
        bindings.mov_w = deng_CreateInputMask(1, DENG_MOUSE_SCROLL_UP);
        bindings.mov_nw = deng_CreateInputMask(1, DENG_MOUSE_SCROLL_DOWN);
        bindings.ch_vcp = deng_CreateInputMask(1, DENG_MOUSE_BTN_2);
        m_cam.setBindings(bindings);

        // Load demo assets
        das_Texture viking_tex = {};
        das_LoadTexture(&viking_tex, "textures/viking_room.tga");
        
        das_Asset viking = {};
        das_LoadAsset(&viking, DAS_ASSET_MODE_UNDEFINED, das_ObjColorData{1.0f, 1.0f, 1.0f, 1.0f}, 
            false, viking_tex.uuid, (char*) "assets/viking.das");

        // Submit assets to the renderer
        m_rend.submitAsset(viking);
        m_rend.submitTexture(viking_tex);

        m_rend.setup(m_cam, m_win); 
    }


    /// General run method
    void ImGUIApplication::run() {
        m_rend.update();
        m_ui_man = std::make_unique<UIManager>(m_rend);
        while(deng_IsRunning()) {
            m_ui_man->updateIO(m_win);
            char buf[128] = { 0 };
            float fl = 0;

            ImGui ::NewFrame();
                // This is a custom test window sample
                ImGui::Begin("Test window", NULL, ImGuiWindowFlags_MenuBar);
                    ImGui::Text("This button click prints out appropriate message regarding the button click");
                    if(ImGui::Button("Test"))
                        LOG("You just clicked a test button");
                ImGui::End();

                // Use this to test functionality
                //ImGui::ShowDemoWindow();
            ImGui::EndFrame();

            ImGui::Render();
            ImDrawData *p_draw_data = ImGui::GetDrawData();

            // Update UI element generation
            m_ui_man->render(p_draw_data, m_win);

            // Update the renderer
            m_rend.update();

            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }

        ImGui::DestroyContext();
    }
}


int main() {
    deng::ImGUIApplication app;
    app.run();

    return EXIT_SUCCESS;
}
