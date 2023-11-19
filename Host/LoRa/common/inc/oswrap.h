#ifndef LORA_OSWRAP_H
#define LORA_OSWRAP_H

#include <chrono>
#include <thread>

#ifdef __cplusplus
typedef std::chrono::steady_clock::time_point systick_t;
#define GET_CURRENT_TIME() std::chrono::steady_clock::now()
#define DURATION_FROM_MS(x) std::chrono::milliseconds((x))
#define MS_FROM_DURATION(x) std::chrono::duration_cast<std::chrono::milliseconds>((x)).count()
#define SLEEP_FOR(x) std::this_thread::sleep_for((x))
#endif

#endif
