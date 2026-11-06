#!/usr/bin/env python3
"""Apply the reviewed, bounded Mali texture-decode worker adaptation.

The upstream submodule is pinned. Refuse to patch an unexpected source version
instead of silently overwriting a later upstream implementation.
"""
from pathlib import Path

TARGET = Path(
    "vendor/XHYN-IV-recomp/glue/rexglue-sdk-main/src/graphics/gta4_native/"
    "native_texture_decode_worker.h"
)

SOURCE = r'''#pragma once
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <mutex>
#include <thread>
#if defined(__ANDROID__)
#include <sched.h>
#endif
#include <rex/graphics/gta4_native/mobile_render_policy.h>

namespace rex::graphics::gta4_native {

// Bounded persistent workers for large CPU texture conversions. The caller
// participates in the work; at most two helper threads are created, avoiding
// the 6-worker burst used by the reference fork on lower-memory phones.
class NativeTextureDecodeWorker {
 public:
  NativeTextureDecodeWorker() {
    const unsigned cores = std::thread::hardware_concurrency();
    const unsigned desired = cores > 2 ? std::min(cores - 1, 2u) : 0u;
    for (unsigned i = 0; i < desired; ++i) {
      try {
        std::thread([this] { Loop(); }).detach();
        ++worker_count_;
      } catch (...) {
        break;
      }
    }
  }

  template <typename Convert>
  bool Run(uint32_t rows, size_t bytes, bool parallel, const Convert& convert) {
    if (!parallel || rows < 16 || bytes < 128 * 1024 || worker_count_ == 0) {
      return convert(0, rows);
    }

    std::lock_guard run_lock(run_mutex_);
    const uint32_t strips =
        std::min<uint32_t>(rows, uint32_t(worker_count_ + 1) * 2);
    const uint32_t rows_per_strip = (rows + strips - 1) / strips;
    std::atomic<bool> ok{true};
    std::exception_ptr failure;
    std::mutex failure_mutex;

    std::function<void(uint32_t)> task =
        [&convert, &ok, &failure, &failure_mutex, rows, rows_per_strip](uint32_t strip) {
          const uint32_t first = strip * rows_per_strip;
          const uint32_t end = std::min(rows, first + rows_per_strip);
          if (first >= end) return;
          try {
            if (!convert(first, end)) ok.store(false, std::memory_order_relaxed);
          } catch (...) {
            ok.store(false, std::memory_order_relaxed);
            std::lock_guard lock(failure_mutex);
            if (!failure) failure = std::current_exception();
          }
        };

    {
      std::lock_guard lock(mutex_);
      job_ = &task;
      next_ = 0;
      total_ = strips;
      remaining_ = strips;
      ++generation_;
    }
    wake_.notify_all();
    Work();

    {
      std::unique_lock lock(mutex_);
      done_.wait(lock, [this] { return remaining_ == 0; });
      job_ = nullptr;
    }
    if (failure) std::rethrow_exception(failure);
    return ok.load(std::memory_order_relaxed);
  }

  size_t worker_count() const { return worker_count_; }

 private:
  void Loop() {
    uint64_t seen = 0;
    for (;;) {
      {
        std::unique_lock lock(mutex_);
        wake_.wait(lock, [this, &seen] { return generation_ != seen; });
        seen = generation_;
      }
      Work();
    }
  }

  void Work() {
    for (;;) {
      const std::function<void(uint32_t)>* job = nullptr;
      uint32_t index = 0;
      {
        std::lock_guard lock(mutex_);
        if (!job_ || next_ >= total_) return;
        job = job_;
        index = next_++;
      }
      (*job)(index);
      {
        std::lock_guard lock(mutex_);
        if (--remaining_ == 0) done_.notify_all();
      }
    }
  }

  size_t worker_count_ = 0;
  std::mutex run_mutex_;
  std::mutex mutex_;
  std::condition_variable wake_;
  std::condition_variable done_;
  const std::function<void(uint32_t)>* job_ = nullptr;
  uint32_t next_ = 0;
  uint32_t total_ = 0;
  uint32_t remaining_ = 0;
  uint64_t generation_ = 0;
};

template <typename Convert>
bool RunNativeTextureDecodeRows(uint32_t rows, size_t bytes, bool enabled,
                                const Convert& convert) {
  if (!enabled ||
      !MobileDecodeParallel(std::thread::hardware_concurrency(), rows, bytes)) {
    return convert(0, rows);
  }
#if defined(__ANDROID__)
  // Respect Android cpusets. A helper inherited onto a restricted/busy CPU set
  // can be slower than serial conversion.
  cpu_set_t allowed;
  if (sched_getaffinity(0, sizeof(allowed), &allowed) == 0 &&
      !MobileDecodeParallel(CPU_COUNT(&allowed), rows, bytes)) {
    return convert(0, rows);
  }
#endif
  // Intentionally process-lifetime: detached workers must not outlive a
  // destructed static object during Android activity teardown.
  static NativeTextureDecodeWorker* worker = new NativeTextureDecodeWorker();
  return worker->Run(rows, bytes, true, convert);
}

}  // namespace rex::graphics::gta4_native
'''

def main():
    if not TARGET.is_file():
        raise SystemExit(f"Expected pinned upstream file is missing: {TARGET}")
    current = TARGET.read_text(encoding="utf-8")
    if "Bounded persistent workers for large CPU texture conversions" in current:
        print("Mali texture decode worker patch already applied.")
        return
    required = (
        '#include "native_pipeline_compiler.h"',
        "NativePipelineCompiler<uint64_t, bool, std::hash<uint64_t>> worker_{1};",
        "RunNativeTextureDecodeRows",
    )
    missing = [marker for marker in required if marker not in current]
    if missing:
        raise SystemExit(
            "Refusing to patch an unexpected upstream version; missing markers: "
            + ", ".join(missing)
        )
    TARGET.write_text(SOURCE, encoding="utf-8")
    print("Applied bounded Mali texture decode worker pool (2 helpers maximum).")

if __name__ == "__main__":
    main()
