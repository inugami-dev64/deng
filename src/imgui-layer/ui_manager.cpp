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


#define __IMGUI_RENDERER_CPP
#include <imgui-layer/ui_manager.h>

namespace deng {

    UIManager::UIManager(deng::Renderer &rend) : m_rend(rend) {
        // Create new ImGui context
        ImGui::CreateContext();
        m_p_io = &ImGui::GetIO();
        
        // Create a texture atlas for ImGui
        das_Texture imgui_atlas = {};
        imgui_atlas.uuid = uuid_Generate();
        m_imgui_atlas = imgui_atlas.uuid;
        m_gui_data.tex_id = imgui_atlas.uuid;
        imgui_atlas.name = (char*) "#ImGui_atlas";
    
        // Temporary width and height variables
        deng_i32_t width, height;
        m_p_io->Fonts->GetTexDataAsRGBA32(&imgui_atlas.pixel_data.pixel_data, &width, &height);
        // Cast the width and height parameters into texture width and height
        imgui_atlas.pixel_data.width = static_cast<deng_ui16_t>(width);
        imgui_atlas.pixel_data.height = static_cast<deng_ui16_t>(height);

        imgui_atlas.pixel_data.size = width * height * 4;
        imgui_atlas.no_reg_cleanup = true;

        das_RealignPixelData(&imgui_atlas, DAS_PIXEL_FORMAT_R8G8B8A8);
        
        // Set the texture ID
        m_p_io->Fonts->SetTexID((void*) imgui_atlas.uuid);

        LOG("Texture width, height: " + std::to_string(imgui_atlas.pixel_data.width) + ":" + 
            std::to_string(imgui_atlas.pixel_data.height));

        // TEMP CODE
        das_Asset test_asset = {};
        test_asset.asset_mode = DAS_ASSET_MODE_2D_TEXTURE_MAPPED;
        test_asset.tex_uuid = m_imgui_atlas;
        test_asset.uuid = uuid_Generate();
        test_asset.indices.n = 6;
        test_asset.indices.pos = (deng_ui32_t*) calloc(test_asset.indices.n, sizeof(deng_ui32_t));
        test_asset.indices.tex = (deng_ui32_t*) calloc(test_asset.indices.n, sizeof(deng_ui32_t));
        test_asset.indices.pos[0] = test_asset.indices.tex[0] = 0;
        test_asset.indices.pos[1] = test_asset.indices.tex[1] = 1;
        test_asset.indices.pos[2] = test_asset.indices.tex[2] = 2;
        test_asset.indices.pos[3] = test_asset.indices.tex[3] = 2;
        test_asset.indices.pos[4] = test_asset.indices.tex[4] = 3;
        test_asset.indices.pos[5] = test_asset.indices.tex[5] = 0;

        test_asset.vertices.v2d.pn = test_asset.vertices.v2d.tn = 4;
        test_asset.vertices.v2d.pos = (das_ObjPosData2D*) calloc(test_asset.vertices.v2d.pn, sizeof(das_ObjPosData2D));
        test_asset.vertices.v2d.tex = (das_ObjTextureData*) calloc(test_asset.vertices.v2d.tn, sizeof(das_ObjTextureData));
        test_asset.vertices.v2d.pos[0] = {-1.0f, -1.0f};
        test_asset.vertices.v2d.pos[1] = {-0.5f, -1.0f};
        test_asset.vertices.v2d.pos[2] = {-0.5f, -0.5f};
        test_asset.vertices.v2d.pos[3] = {-1.0f, -0.5f};

        test_asset.vertices.v2d.tex[0] = { 0.0f, 0.0f };
        test_asset.vertices.v2d.tex[1] = { 1.0f, 0.0f };
        test_asset.vertices.v2d.tex[2] = { 1.0f, 1.0f };
        test_asset.vertices.v2d.tex[3] = { 0.0f, 1.0f };

        test_asset.diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
        test_asset.is_shown = true;

        test_asset.is_transformed = false;
        test_asset.force_unmap = false;

        m_rend.pushTexture(imgui_atlas);
        m_rend.pushAsset(test_asset);
        m_rend.setUIDataPtr(&m_gui_data);
    }


    UIManager::~UIManager() {
        //delete [] m_gui_data.verts;
        //delete [] m_gui_data.ind;
    }


    // Update IO device input
    void UIManager::updateIO(const deng::Window &win) {
        m_p_io->DeltaTime = 1 / 60.0f;
        m_p_io->DisplaySize.x = static_cast<deng_vec_t>(win.getSize().first);
        m_p_io->DisplaySize.y = static_cast<deng_vec_t>(win.getSize().second);
        m_p_io->MousePos.x = static_cast<deng_vec_t>(win.getMPos().first);
        m_p_io->MousePos.y = static_cast<deng_vec_t>(win.getMPos().second);

        m_p_io->MouseDown[0] = __deng_FindKeyStatus(DENG_KEY_UNKNOWN, DENG_MOUSE_BTN_1, DENG_INPUT_TYPE_MOUSE,
            DENG_INPUT_EVENT_TYPE_ACTIVE);
        m_p_io->MouseDown[1] = __deng_FindKeyStatus(DENG_KEY_UNKNOWN, DENG_MOUSE_BTN_2, DENG_INPUT_TYPE_MOUSE,
            DENG_INPUT_EVENT_TYPE_ACTIVE);
    }


    /// Update ImGui draw content according to the changed data
    void UIManager::render(ImDrawData *p_draw_data, const Window &win) {
        std::this_thread::sleep_for(std::chrono::microseconds(1000));

        // For each command list in the list output its vertices to the renderer
        for(deng_ui32_t i = 0; i < p_draw_data->CmdListsCount; i++) {
            const ImDrawList *cmd_list = p_draw_data->CmdLists[i];
            ImDrawVert *verts = cmd_list->VtxBuffer.Data;
            ImDrawIdx *inds = cmd_list->IdxBuffer.Data;

            // For each vertex, convert its pixel size to vector size
            for(deng_ui32_t j = 0; j < cmd_list->VtxBuffer.Size; j++) {
                verts[j].pos.x = dengMath::Conversion::mouseCoordToVecCoord(verts[j].pos.x, static_cast<deng_px_t>(win.getSize().first));
                verts[j].pos.y = dengMath::Conversion::mouseCoordToVecCoord(verts[j].pos.y, static_cast<deng_px_t>(win.getSize().second));
            }

            m_gui_data.verts = verts;
            m_gui_data.vert_c = cmd_list->VtxBuffer.Size;

            m_gui_data.ind = inds;
            m_gui_data.ind_c = cmd_list->IdxBuffer.Size;

            // ImGui vertices buffer area offset
            size_t offset = 0;
            
            // For each command buffer in the current command list set its vertices and indices
            m_gui_data.entities.resize(cmd_list->CmdBuffer.Size);
            for(deng_ui32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
                const ImDrawCmd *p_cmd = &cmd_list->CmdBuffer[j];

                // Check if user callback should be respected
                if(p_cmd->UserCallback) 
                    p_cmd->UserCallback(cmd_list, p_cmd);

                // Perform buffer data update 
                else {
                    m_gui_data.entities[j].buf_offset = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert) + 
                        p_cmd->IdxOffset * sizeof(ImDrawIdx);
                    m_gui_data.entities[j].ind = inds + p_cmd->IdxOffset;
                    m_gui_data.entities[j].ind_c = p_cmd->ElemCount;
                }
            }
        }
        
        // Update vertices data
        m_rend.uiUpdateData();
    }
}