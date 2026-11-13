#!/usr/bin/env python3
"""Add observational Android SDL audio diagnostics to the pinned XHYN runtime."""
from pathlib import Path
import subprocess
import sys

PIN = "be597d42d12abb04a552fd87e3853028499ae835"
root = Path(sys.argv[1]).resolve()
actual = subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
if actual != PIN:
    raise SystemExit(f"Refusing unexpected XHYN source {actual}; expected {PIN}")
target = root / "glue/rexglue-sdk-main/src/audio/sdl/sdl_audio_driver.cpp"
s = target.read_text(encoding="utf-8")
if "GTA4Universal-Audio" in s:
    print("Android audio diagnostics already applied.")
    raise SystemExit(0)

def once(old, new, label):
    global s
    if s.count(old) != 1:
        raise SystemExit(f"Refusing audio patch at {label}: expected one anchor, found {s.count(old)}")
    s = s.replace(old, new, 1)

once("#include <SDL3/SDL.h>\n",
     "#include <SDL3/SDL.h>\n#if defined(__ANDROID__)\n#include <android/log.h>\n#endif\n", "include")
once("namespace {\n\nstruct SDLSubsystemResult",
     """namespace {

#if defined(__ANDROID__)
void UniversalAudioDiag(const char* event, const char* detail = "") {
  __android_log_print(ANDROID_LOG_WARN, "GTA4Universal-Audio", "event=%s detail=%s", event, detail);
}
#else
void UniversalAudioDiag(const char*, const char* = "") {}
#endif

struct SDLSubsystemResult""", "helper")
once("bool SDLAudioDriver::Initialize() {\n  low_latency_=REXCVAR_GET(audio_low_latency);",
     """bool SDLAudioDriver::Initialize() {
  low_latency_=REXCVAR_GET(audio_low_latency);
  UniversalAudioDiag("initialize_begin", low_latency_ ? "low_latency=1" : "low_latency=0");""", "start")
once("  if (!subsystem_result.success) {\n",
     """  if (!subsystem_result.success) {
    UniversalAudioDiag("sdl_subsystem_failed", subsystem_result.error.c_str());
""", "SDL failure")
once("  sdl_initialized_ = true;\n",
     """  sdl_initialized_ = true;
  UniversalAudioDiag("sdl_subsystem_ready");
""", "SDL ready")
once("  if (!sdl_stream_) {\n    if (desired_spec.channels != 2) {",
     """  if (!sdl_stream_) {
    UniversalAudioDiag("primary_stream_open_failed", SDL_GetError());
    if (desired_spec.channels != 2) {""", "stream open")
once("    if (!sdl_stream_) {\n      REXAPU_ERROR(\n",
     """    if (!sdl_stream_) {
      UniversalAudioDiag("stereo_stream_open_failed", SDL_GetError());
      REXAPU_ERROR(
""", "stereo fallback")
once("  if (!SDL_ResumeAudioStreamDevice(sdl_stream_)) {\n",
     """  if (!SDL_ResumeAudioStreamDevice(sdl_stream_)) {
    UniversalAudioDiag("stream_resume_failed", SDL_GetError());
""", "resume failure")
once("  return true;\n}\n\nvoid SDLAudioDriver::SubmitFrame",
     """  UniversalAudioDiag("stream_resumed");
  return true;
}

void SDLAudioDriver::SubmitFrame""", "resume success")
target.write_text(s, encoding="utf-8")
print("Applied observational Android SDL audio diagnostics.")
