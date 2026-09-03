#pragma once

#include "cielvox/config.h"
#include "cielvox/common.h"
#include "cielvox/gguf_loader.h"
#include "cielvox/tokenizer.h"

#include <memory>
#include <string>

namespace cielvox {

struct BreezeModel {
    Backend backend;
    GGUFModel gg;
    BreezeConfig cfg;
    Tokenizer tok;

    bool load(const std::string & path, bool prefer_gpu);
    void free();

    ggml_tensor * w(const std::string & name) const { return gg.get(name); }
    ggml_tensor * wopt(const std::string & name) const { return gg.find(name); }
};

}
