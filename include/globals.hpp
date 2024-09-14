#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <drf_client.hpp>
#include <mutex>
#include <nexus/Nexus.h>
#include <nlohmann/json.hpp>
#include <queue>
#include <string>

// handle to self hmodule
extern HMODULE self_module;
// addon definition
extern AddonDefinition addon_def;
// addon api
extern AddonAPI *api;

extern char addon_name[];

extern HWND game_handle;

extern DrfClient *drf_client;

extern std::mutex to_process_mutex;
extern std::queue<nlohmann::json> to_process;

#endif // GLOBALS_HPP
