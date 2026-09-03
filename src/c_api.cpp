#include "cielvox/breeze.h"
#include "cielvox/audio.h"
#include "cielvox/generation.h"

#include <exception>
#include <string>
#include <vector>

using namespace cielvox;

struct cielvox_context {
    BreezeModel model;
    MimiCodec codec;
};

static std::string g_error;

static GenRequest to_req(const cielvox_request * r) {
    GenRequest g;
    g.text = r->text ? r->text : "";
    g.instruction = r->instruction ? r->instruction : "Speak clearly and naturally.";
    g.ref_text = r->ref_text ? r->ref_text : "";
    if (r->ref_audio && r->ref_audio_len > 0)
        g.ref_audio.assign(r->ref_audio, r->ref_audio + r->ref_audio_len);
    g.cfg_scale = r->cfg_scale > 0 ? r->cfg_scale : 1.0f;
    g.seed = r->seed;
    g.max_new_tokens = r->max_new_tokens;
    if (r->split_chars != 0) g.split_chars = r->split_chars < 0 ? 0 : r->split_chars;
    g.temperature = r->temperature;
    g.top_k = r->top_k;
    g.top_p = r->top_p;
    g.repetition_penalty = r->repetition_penalty;
    return g;
}

cielvox_context * cielvox_init(const char * gguf_path, int use_gpu) {
    cielvox_context * c = new cielvox_context();
    try {
        if (!c->model.load(gguf_path, use_gpu != 0)) {
            g_error = "failed to load model";
            delete c;
            return nullptr;
        }
    } catch (const std::exception & e) {
        g_error = e.what();
        delete c;
        return nullptr;
    }
    c->codec.init(c->model);
    return c;
}

void cielvox_free(cielvox_context * ctx) {
    if (ctx) {
        ctx->model.free();
        delete ctx;
    }
}

int cielvox_sample_rate(cielvox_context * ctx) {
    return ctx ? ctx->model.cfg.sample_rate : 0;
}

int cielvox_generate(cielvox_context * ctx, const cielvox_request * req, cielvox_audio_cb cb, void * user) {
    try {
        GenRequest g = to_req(req);
        generate(ctx->model, ctx->codec, g, [&](const float * s, int n) {
            return cb ? cb(s, n, user) == 0 : true;
        });
    } catch (const std::exception & e) {
        g_error = e.what();
        return 1;
    }
    return 0;
}

int cielvox_generate_wav(cielvox_context * ctx, const cielvox_request * req, const char * out_path) {
    try {
        GenRequest g = to_req(req);
        std::vector<float> audio;
        generate(ctx->model, ctx->codec, g, [&](const float * s, int n) {
            audio.insert(audio.end(), s, s + n);
            return true;
        });
        if (!write_wav(out_path, audio, ctx->model.cfg.sample_rate)) {
            g_error = "failed to write wav";
            return 1;
        }
    } catch (const std::exception & e) {
        g_error = e.what();
        return 1;
    }
    return 0;
}

const char * cielvox_last_error(void) {
    return g_error.c_str();
}
