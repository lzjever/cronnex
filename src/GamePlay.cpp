#pragma warning( disable : 4576)
#pragma warning( disable : 4244)
#include "raylib.h"
#include "nes/Bus.h"
#include "nes/APU2A03.h"
#include "nes/PPU2C02.h"
#include "nes/CPU6502.h"
#include "nes/Cartridge.h"
#include <memory>
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 256;
    const int screenHeight = 240;
    InitWindow(screenWidth, screenHeight, "Cronnex Gameplay");
    SetTraceLogLevel(LOG_ERROR);

    std::shared_ptr<Bus> nes;
    std::shared_ptr<Cartridge> cart;
    cart = std::make_shared<Cartridge>("E:\\works\\test_bin\\Ice Climber (JE).nes");

    nes = std::make_shared<Bus>();
    nes->connect_cpu(std::make_shared<CPU6502>());
    nes->connect_ppu(std::make_shared<PPU2C02>());

    if (!cart->is_valid())
        return false;

    // Insert into NES
    nes->insert_cartridge(cart);

    // Reset NES
    nes->reset();

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    BeginTextureMode(target);
    ClearBackground(BLACK);
    EndTextureMode();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        //do { nes->clock(); } while (!nes->ppu_->is_frame_complete_);
        nes->run_frame();
        Image screen_img = LoadImagePro(nes->ppu_->get_video_buffer(), screenWidth, screenHeight, UNCOMPRESSED_R8G8B8A8);
        Texture2D texture_screen = LoadTextureFromImage(screen_img);
        UnloadImage(screen_img);

        BeginTextureMode(target);
            DrawTextureRec(texture_screen, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, (Vector2) { 0.0f, 0.0f }, WHITE);
        EndTextureMode();

        UnloadTexture(texture_screen);


        BeginDrawing();
            //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            DrawTextureRec(target.texture, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, (Vector2) { 0.0f, 0.0f }, WHITE);
        EndDrawing();

        nes->ppu_->is_frame_complete_ = false;
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);    // Unload render texture
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}