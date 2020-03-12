#pragma warning( disable : 4576)
#pragma warning( disable : 4244)
#include "raylib.h"
#include "nes/Bus.h"
#include "nes/APU2A03.h"
#include "nes/PPU2C02.h"
#include "nes/CPU6502.h"
#include "nes/Cartridge.h"
#include <memory>
#include <list>
#include <vector>
#include <string>
#include <cstring>


// NOTE: Gamepad name ID depends on drivers and OS
#if defined(PLATFORM_RPI)
#define XBOX360_NAME_ID     "Microsoft X-Box 360 pad"
#define PS3_NAME_ID         "PLAYSTATION(R)3 Controller"
#else
#define XBOX360_NAME_ID     "Xbox 360 Controller"
#define PS3_NAME_ID         "PLAYSTATION(R)3 Controller"
#endif
AudioStream stream;
std::list<vector<int16_t> > sound_buffer;
bool sound_sample_callback(int16_t* data, int32_t data_size)
{
    TraceLog(LOG_INFO,"Sound sample has arrived, size : %d . ", data_size);
    if (IsAudioStreamProcessed(stream) && (sound_buffer.size() == 0))
    {
        UpdateAudioStream(stream, data, data_size);
    }
    else
    {
        vector<int16_t> sample_in(data_size);
        std::memcpy(sample_in.data(), data, data_size * sizeof(int16_t));
        sound_buffer.push_back(sample_in);
    }


    return true;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 256; //1024
    const int screenHeight = 240; //960
    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Set MSAA 4X hint before windows creation

    InitWindow(screenWidth, screenHeight, "Cronnex Gameplay");
    //SetTraceLogLevel(LOG_ERROR);

    InitAudioDevice();
    if (IsAudioDeviceReady())
    {
        SetMasterVolume(1.0f);
        stream = InitAudioStream(44100, 16, 1);
        PlayAudioStream(stream);
    }
    TraceLog(LOG_INFO , "Audio device is ready.");

    std::shared_ptr<Bus> nes = std::make_shared<Bus>();

    nes->connect_cpu(std::make_shared<CPU6502>());
    nes->connect_ppu(std::make_shared<PPU2C02>());
    nes->connect_apu(std::make_shared<APU2A03>(sound_sample_callback));
    TraceLog(LOG_INFO , "Finish setting up emulator console.");

    //auto a = std::make_shared<APU2A03>(sound_sample_callback);

    // Insert cart into NES
    std::shared_ptr<Cartridge> cart = std::make_shared<Cartridge>(
        std::string("../../test_bin/Ice Climber (JE).nes"));
    if (!cart->is_valid())
        return false;
    nes->insert_cartridge(cart);

    // Reset NES
    nes->reset();

    TraceLog(LOG_INFO , "Finish setting up cartridge.");

    RenderTexture2D target = LoadRenderTexture(256, 240);
    SetTextureFilter(target.texture, FILTER_TRILINEAR);
    BeginTextureMode(target);
    ClearBackground(BLACK);
    EndTextureMode();

    SetTargetFPS(60);
    TraceLog(LOG_INFO , "Start rendering game window.");
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //update sound
        if (IsAudioStreamProcessed(stream) && (sound_buffer.size() > 0))
        {
            std::vector<int16_t>& sample_in = *sound_buffer.begin();
            UpdateAudioStream(stream, sample_in.data(), sample_in.size());
            sound_buffer.pop_front();
        }

        if (IsKeyPressed(KEY_R))
        {
            nes->reset();
        }
        if (IsKeyPressed(KEY_ONE))
        {
            cart = std::make_shared<Cartridge>(
                std::string("../../test_bin/Ice Climber (JE).nes"));
            if (!cart->is_valid())
                return false;
            nes->insert_cartridge(cart);
            nes->reset();
        }
        if (IsKeyPressed(KEY_TWO))
        {
            cart = std::make_shared<Cartridge>(
                std::string("../../test_bin/Donkey Kong (JU).nes"));
            if (!cart->is_valid())
                return false;
            nes->insert_cartridge(cart);
            nes->reset();
        }
        if (IsKeyPressed(KEY_THREE))
        {
            cart = std::make_shared<Cartridge>(
                std::string("../../test_bin/Super Mario Bros (J).nes"));
            if (!cart->is_valid())
                return false;
            nes->insert_cartridge(cart);
            nes->reset();
        }
        if (IsKeyPressed(KEY_FOUR))
        {
            cart = std::make_shared<Cartridge>(
                std::string("../../test_bin/nestest.nes"));
            if (!cart->is_valid())
                return false;
            nes->insert_cartridge(cart);
            nes->reset();
        }


        nes->controller_[0] = 0x00;
        nes->controller_[0] |= IsKeyDown(KEY_K) ? 0x80 : 0x00;      //A
        nes->controller_[0] |= IsKeyDown(KEY_J) ? 0x40 : 0x00;      //B
        nes->controller_[0] |= IsKeyDown(KEY_T) ? 0x20 : 0x00;      //select
        nes->controller_[0] |= IsKeyDown(KEY_Y) ? 0x10 : 0x00;      //start
        nes->controller_[0] |= IsKeyDown(KEY_W) ? 0x08 : 0x00;      //up
        nes->controller_[0] |= IsKeyDown(KEY_S) ? 0x04 : 0x00;      //down
        nes->controller_[0] |= IsKeyDown(KEY_A) ? 0x02 : 0x00;      //left
        nes->controller_[0] |= IsKeyDown(KEY_D) ? 0x01 : 0x00;      //right

       // while (!nes->ppu_->is_frame_complete_)
       //     nes->clock();

        //nes->run_frame();


        Image screen_img = LoadImagePro(nes->ppu_->get_video_buffer(), 256, 240, UNCOMPRESSED_R8G8B8A8);
        Texture2D texture_screen = LoadTextureFromImage(screen_img);

        ///////////////////////////////// Render /////////////////////////////////
        BeginTextureMode(target);
            ClearBackground(BLACK);
            DrawTextureRec(texture_screen, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, (Vector2) { 0.0f, 0.0f }, WHITE);
        EndTextureMode();

        UnloadImage(screen_img);
        UnloadTexture(texture_screen);


        BeginDrawing();
            ClearBackground(BLACK);
            //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
            //DrawTextureRec(target.texture, (Rectangle) { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height }, (Vector2) { 0.0f, 0.0f }, WHITE);
            DrawTexturePro(target.texture, 
                (Rectangle) { 0.0f, 0.0f, 256.0f, 240.0f },
                (Rectangle) { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
                (Vector2) { 0.0f, 0.0f }, 0.0f, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(target);    // Unload render texture

    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context

    return 0;
}