#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <mutex>
#include <nlohmann/json.hpp>

namespace Settings
{

void load(const std::filesystem::path &path);
void save(const std::filesystem::path &path);

extern nlohmann::json json_settings;
extern std::filesystem::path settings_path;
extern std::mutex mutex;

extern bool is_addon_enabled;
extern std::string drf_token;

extern const char *IS_ADDON_ENABLED;
extern const char *DRF_TOKEN;
} // namespace Settings

#endif // SETTINGS_HPP
