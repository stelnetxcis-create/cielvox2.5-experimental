# Voice Design Skill — CielVox 2.5

How to design a voice from scratch using CielVox 2.5. Not cloning. Not voice conversion.
Voice design = give the model a character description, get a new voice that matches.

## The craft

Voice design is about giving the model enough detail to invent a voice, but not so much
that it gets confused. Three things matter:

1. **Instruction** — who is speaking, not what they say
2. **Text** — what they say, with vocal event tags baked in
3. **cfg-scale** — how hard to push toward the instruction

## Instruction format

Write it like a casting director, not a chatbot prompt:

- Start with the core identity: "A cold, emotionless clockwork puppet"
- Add delivery texture: "Precise, mechanical, flat delivery with zero warmth"
- Add vocal quality: "Slight metallic undercurrent, unnatural pauses between phrases"
- One paragraph, 2-4 sentences. Anything longer gets ignored or diluted.

Bad: "You are Sandrone from Genshin Impact. Please speak like her."
Good: "A cold, emotionless clockwork puppet. Precise, mechanical, flat delivery
with zero warmth. Every word sounds like it's coming from something that shouldn't
be alive. Slight metallic undercurrent, unnatural pauses between phrases."

## Text format

Plain text with vocal event tags in round brackets. Tags are free-form — the model
was trained on descriptive tags, not a fixed list.

Reliable tags: (sigh), (laugh), (cough), (clears throat), (gasp), (whispering),
(nervous chuckle), (mechanical whir)

Use tags at natural pause points — start of a sentence, before a key phrase.
Don't stack more than 2-3 per generation.

## cfg-scale

- **1.0** (default) — skip the second pass. Fastest. Use for neutral, calm speech.
- **2.0 - 3.0** — push harder toward the instruction. Needed when vocal events
  must actually fire or the voice is ignoring the description.
- Past 3.0 the output picks up audible harshness. Not worth it.

For character voices with strong personality: start at 1.5, adjust up if the
voice comes out too neutral, down if it sounds strained.

## The recipe

```bash
time env -u LD_LIBRARY_PATH ./build/cielvox-cli \
  /path/to/cielvox2.5-q8_0.gguf \
  --text "your speech with (vocal tags)" \
  --instruction "character description paragraph" \
  --cfg-scale 1.5 \
  --timings \
  --output output.wav
```

No ref-audio, no ref-text, no --voice. Pure design. The model invents the voice
from the instruction alone.

## Example: Sandrone (Genshin Impact)

```bash
time env -u LD_LIBRARY_PATH ./build/cielvox-cli \
  /mnt/storage/.stelos/stelnet/stelnet_models/TTS/cielvox2.5/cielvox2.5-q8_0.gguf \
  --text "(mechanical whir) (flat tone) How... predictable. You reach for the lever,
the switch, the thing that should grant you control. (synthetic sigh) But you are so
very wrong." \
  --instruction "A cold, emotionless clockwork puppet. Precise, mechanical, flat
delivery with zero warmth. Every word sounds like it's coming from something that
shouldn't be alive. Slight metallic undercurrent, unnatural pauses between phrases." \
  --cfg-scale 1.5 \
  --timings \
  --output sandrone_design.wav
```

What this does:
- Instruction: paints Sandrone as a clockwork automaton — cold, mechanical, not-quite-human
- Text: uses (mechanical whir) and (synthetic sigh) as vocal events, ellipsis for
  unnatural pauses, short declarative sentences matching her monotone speech pattern
- cfg-scale 1.5: enough to push the voice toward cold/mechanical without harshness

## Tips

- Short text = cleaner voice. Under 200 characters for first attempts.
- Generate 3-4 takes with different seeds, pick the best one. Same seed = same output.
- If the voice comes out too warm/neutral, raise cfg-scale to 2.0.
- If it sounds strained or harsh, drop cfg-scale to 1.0.
- Vocal events need higher cfg-scale (2.0+) to actually fire. Don't waste 1.0 on them.
- Once you land a good instruction, save it. Reuse it with different text for
  consistent character voices across multiple generations.
