#ifndef CIELVOX_H
#define CIELVOX_H

#include <stddef.h>

#if defined(_WIN32)
#ifdef CIELVOX_BUILD_SHARED
#define CIELVOX_API __declspec(dllexport)
#else
#define CIELVOX_API
#endif
#else
#define CIELVOX_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cielvox_context cielvox_context;

// return 0 to continue, non-zero to stop generation
typedef int (*cielvox_audio_cb)(const float * samples, int n_samples, void * user);

typedef struct {
    const char * text;
    const char * instruction;   // null for a neutral default
    const char * ref_text;      // null when not cloning
    const float * ref_audio;    // mono 24 kHz samples, null for voice design
    int ref_audio_len;
    float cfg_scale;
    int seed;
    int max_new_tokens;         // 0 uses the model default
    int split_chars;            // 0 uses the default, negative keeps long text in a single pass
    // sampling. zero on any of these keeps whatever the gguf was built with
    float temperature;
    int top_k;
    float top_p;
    float repetition_penalty;
} cielvox_request;

CIELVOX_API cielvox_context * cielvox_init(const char * gguf_path, int use_gpu);
CIELVOX_API void cielvox_free(cielvox_context * ctx);
CIELVOX_API int cielvox_sample_rate(cielvox_context * ctx);

// streams audio chunks to the callback; returns 0 on success
CIELVOX_API int cielvox_generate(cielvox_context * ctx, const cielvox_request * req,
                               cielvox_audio_cb cb, void * user);

// convenience: generate and write a 16-bit PCM WAV; returns 0 on success
CIELVOX_API int cielvox_generate_wav(cielvox_context * ctx, const cielvox_request * req,
                                   const char * out_path);

CIELVOX_API const char * cielvox_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
