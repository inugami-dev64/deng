#include "../core/api_core.h"
#define SANDBOX_BACKGROUND {0.1411765f, 0.0431372f, 0.2313725f, 1.0f}

std::vector<deng_Asset> assets(1);
std::vector<dengMath::ModelMatrix> am(2);
std::vector<deng_Texture> textures(2);

int main() {
    // Create initialiser class objects
    deng::WindowWrap window(1440, 1080, "Game!");
    deng::Renderer rend;
    
    rend.setHints (
        DENG_RENDERER_HINT_SHOW_FPS_COUNTER
    );

    rend.submitTextureFile (
        "textures/viking_room.tga", 
        "viking_room"
    );

    // Read assets
    dasReadAsset (
        &assets[0], 
        "assets/viking_room.das", 
        DENG_ASSET_MODE_3D_TEXTURE_MAPPED
    );

    assets[0].id = (char*) "viking_room";
    assets[0].is_shown = true;
    
    am[0].setRotation(0.0f, 0.0f, 0.0f);        
    am[0].setTransformation(0.0f, -0.5f, 1.0f);
    am[0].setScale(2.0f, 2.0f, 2.0f);

    dengMath::applyModelMatrix (
        assets[0], 
        am[0].getModelMatrix()
    );
    
    // Submit data
    rend.submitAssets(assets.data(), assets.size());

    // Run renderer
    rend.initRenderer (
        &window, 
        DENG_RENDERER_USAGE_MAP_EDITOR,
        SANDBOX_BACKGROUND
    );    
    rend.run();

    return 0;
}