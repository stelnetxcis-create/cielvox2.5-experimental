<div align="center">

# CielVox 2.5

Local-first text-to-speech, optimized for AMD APUs with Vulkan.
Built on ggml by Cyna. Shaped by StelNet.

<a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache_2.0-1f6feb?style=for-the-badge" alt="License"></a>
<img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++17">
<img src="https://img.shields.io/badge/Vulkan-AMD-AC162C?style=for-the-badge&logo=vulkan&logoColor=white" alt="Vulkan">
<a href="https://huggingface.co/Xenna/cielvox2.5"><img src="https://img.shields.io/badge/GGUF_weights-FFD21E?style=for-the-badge&logo=huggingface&logoColor=black" alt="GGUF weights"></a>

</div>

## What it does

| Mode | Input | Output |
| --- | --- | --- |
| **Voice design** | Text description of a voice | New voice matching that description |
| **Voice cloning** | Reference clip + transcript | Speech in that voice |
| **Voice direction** | Reference clip + instruction | Same voice, different tone/pace |
| **Voice conversion** | Recording + target voice | Respoken in the target voice |

## Build

```bash
git clone --recursive https://github.com/stelnetxcis-create/cielvox2.5-experimental.git
cd cielvox2.5-experimental
cmake -B build -DCMAKE_BUILD_TYPE=Release 
cmake --build build -j$(nproc)
```

CPU-only? Add `-DCIELVOX_VULKAN=OFF`. Outputs: `cielvox-cli`, `cielvox-server`, `cielvox-convert`, `cielvox-quantize`.

## CLI

```bash
# voice design
time env -u LD_LIBRARY_PATH ./build/cielvox-cli /path/to/cielvox2.5-q8_0.gguf \
  --text "(sigh) Welcome aboard. Your journey begins now." \
  --instruction "A warm, thoughtful young woman with a clear, calm delivery." \
  --output design.wav

# voice clone
time env -u LD_LIBRARY_PATH ./build/cielvox-cli /path/to/cielvox2.5-q8_0.gguf \
  --text "It is good to hear your voice again." \
  --ref-audio reference.wav --ref-text "Exact transcript of the reference." \
  --output clone.wav

# voice direction
time env -u LD_LIBRARY_PATH ./build/cielvox-cli /path/to/cielvox2.5-q8_0.gguf \
  --text "(clears throat) We need to talk." \
  --instruction "Speak slowly with a restrained, serious tone." \
  --ref-audio reference.wav --ref-text "Exact transcript of the reference." \
  --output direction.wav
```

## Vocal events

Inline tags in round brackets produce non-speech sounds. `(laugh)`, `(sigh)`, `(cough)`, `(clears throat)`, `(gasp)`, `(whispering)`, `(nervous chuckle)` — free form. Most reliable with `--cfg-scale 2` to `3` to actually fire.

```bash
time env -u LD_LIBRARY_PATH ./build/cielvox-cli /path/to/cielvox2.5-q8_0.gguf \
  --text "(nervous chuckle) I am sure it is nothing to worry about." \
  --instruction "An anxious man trying to sound casual." \
  --cfg-scale 2.5 --output event.wav
```

## Authors

StelNet & Cyna
