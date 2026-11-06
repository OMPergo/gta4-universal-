#!/usr/bin/env python3
"""Add Android-only SDL audio startup diagnostics to the pinned XHYN source.

This is deliberately observational: it does not change audio format, buffering,
device selection, queue policy, or decoder behavior.
"""
from pathlib import Path

TARGET = Path(
    "vendor/XHYN-IV-recomp/glue/rexglue-sdk-main/src/audio/sdl/"
    "sdl_audio_driver.cpp"
)

def replace_once(source: str, old: str, new: str, label: str) -> str:
    count = source.count(old)
    if count != 1:
        raise SystemExit(f"Refusing audio diagnostic patch: {label}: expected 1 anchor, found {count}")
    return source.replace(old, new, 1)

def main() -> None:
    if not TARGET.is_file():
        raise SystemExit(f"Missing pinned SDL audio source: {TARGET}")
    source = TARGET.read_text(encoding="utf-8")
    marker = "LibertyRecomp-Audio"
    if marker in source:
        print("Android SDL audio diagnostics already applied.")
        return

    source = replace_once(
        source,
        "#include <SDL3/SDL.h>\n",
        "#include <SDL3/SDL.h>\n#if defined(__ANDROID__)\n#include <android/log.h>\n#endif\n",
        "Android log include",
    )
    source = replace_once(
        source,
        "namespace rex::audio::sdl {\n\nnamespace {\n",
        """namespace rex::audio::sdl {

namespace {

#if defined(__ANDROID__)
void AndroidAudioDiag(const char* event, const char* detail = "") {
  __android_log_print(ANDROID_LOG_WARN, "LibertyRecomp-Audio",
                      "event=%s detail=%s", event, detail);
}
#else
void AndroidAudioDiag(const char*, const char* = "") {}
#endif
""",
        "diagnostic helper",
    )
    source = replace_once(
        source,
        "bool SDLAudioDriver::Initialize() {\n  low_latency_=REXCVAR_GET(audio_low_latency);",
        """bool SDLAudioDriver::Initialize() {
  low_latency_=REXCVAR_GET(audio_low_latency);
  AndroidAudioDiag("initialize_begin", low_latency_ ? "low_latency=1" : "low_latency=0");""",
        "initialization entry",
    )
    source = replace_once(
        source,
        "  if (!subsystem_result.success) {\n",
        """  if (!subsystem_result.success) {
    AndroidAudioDiag("sdl_subsystem_failed", subsystem_result.error.c_str());
""",
        "subsystem failure",
    )
    source = replace_once(
        source,
        "  sdl_initialized_ = true;\n",
        """  sdl_initialized_ = true;
  AndroidAudioDiag("sdl_subsystem_ready");
""",
        "subsystem success",
    )
    source = replace_once(
        source,
        "  if (!sdl_stream_) {\n    if (desired_spec.channels != 2) {",
        """  if (!sdl_stream_) {
    AndroidAudioDiag("primary_stream_open_failed", SDL_GetError());
    if (desired_spec.channels != 2) {""",
        "primary stream failure",
    )
    source = replace_once(
        source,
        "    if (!sdl_stream_) {\n      REXAPU_ERROR(\n",
        """    if (!sdl_stream_) {
      AndroidAudioDiag("stereo_stream_open_failed", SDL_GetError());
      REXAPU_ERROR(
""",
        "stereo stream failure",
    )
    source = replace_once(
        source,
        "  if (!SDL_ResumeAudioStreamDevice(sdl_stream_)) {\n",
        """  if (!SDL_ResumeAudioStreamDevice(sdl_stream_)) {
    AndroidAudioDiag("stream_resume_failed", SDL_GetError());
""",
        "resume failure",
    )
    source = replace_once(
        source,
        "  return true;\n}\n\nvoid SDLAudioDriver::SubmitFrame",
        """  AndroidAudioDiag("stream_resumed");
  return true;
}

void SDLAudioDriver::SubmitFrame""",
        "resume success",
    )
    TARGET.write_text(source, encoding="utf-8")
    print("Applied observational Android SDL audio startup diagnostics.")

if __name__ == "__main__":
    main()
