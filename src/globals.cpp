#include <globals.hpp>

HMODULE self_module = nullptr;
AddonDefinition addon_def{};
AddonAPI *api = nullptr;
char addon_name[] = "DRF Frontend";
HWND game_handle = nullptr;

DrfClient *drf_client = nullptr;

std::mutex to_process_mutex;
std::queue<nlohmann::json> to_process;