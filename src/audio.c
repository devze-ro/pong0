#include "defines.h"

#define MINIAUDIO_IMPLEMENTATION
#include "../lib/miniaudio.h"

global ma_engine g_engine;
global ma_sound g_ball_hit_sound;
global ma_sound g_score_sound;

internal void load_sound_effect(u8 *filePath, ma_sound* pSound) {
    ma_sound_init_from_file(&g_engine, filePath, MA_SOUND_FLAG_DECODE, NULL, NULL,
            pSound);
}

internal void init_audio() {
    ma_result result;

    ma_engine_config engineConfig = ma_engine_config_init();
    result = ma_engine_init(&engineConfig, &g_engine);
    if (result != MA_SUCCESS) {
        printf("Audio engine initialization failed!");
    }

    load_sound_effect("assets/hit.mp3", &g_ball_hit_sound);
    load_sound_effect("assets/score.mp3", &g_score_sound);
}

internal void play_sound(ma_sound* pSound) {
    if (pSound != NULL)
    {
        ma_sound_start(pSound);
    }
}

internal void play_ball_hit_sound() {
    play_sound(&g_ball_hit_sound);
}

internal void play_score_sound() {
    play_sound(&g_score_sound);
}

internal void shutdown_audio() {
    ma_sound_uninit(&g_ball_hit_sound);
    ma_sound_uninit(&g_score_sound);
    ma_engine_uninit(&g_engine);
}

