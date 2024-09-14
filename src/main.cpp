#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <nexus/Nexus.h>
#include <settings.hpp>

void addon_load(AddonAPI *api_p);
void addon_unload();
void addon_render();
void addon_options();
// UINT wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY dll_main(const HMODULE hModule, const DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        self_module = hModule;
        break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        break;
    }
    return TRUE;
}

// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" __declspec(dllexport) AddonDefinition *GetAddonDef()
{
    addon_def.Signature = -91578884;
    addon_def.APIVersion = NEXUS_API_VERSION;
    addon_def.Name = addon_name;
    addon_def.Version.Major = 0;
    addon_def.Version.Minor = 1;
    addon_def.Version.Build = 0;
    addon_def.Version.Revision = 0;
    addon_def.Author = "Seres67";
    addon_def.Description = "A simple frontend for the DRF addon.";
    addon_def.Load = addon_load;
    addon_def.Unload = addon_unload;
    addon_def.Flags = EAddonFlags_None;
    addon_def.Provider = EUpdateProvider_GitHub;
    addon_def.UpdateLink = "https://github.com/Seres67/nexus_drf_ui";

    return &addon_def;
}

typedef struct
{
    int id;
    int quantity;
} item;

typedef struct
{
    std::string type;
    std::string character;
    std::vector<item> drops;
    std::vector<item> currencies;
    std::string timestamp;
} message;

std::atomic<bool> process_thread_running = false;
void process_messages()
{
    process_thread_running = true;
    while (process_thread_running) {
        if (to_process.empty())
            continue;
        nlohmann::json json;
        {
            std::lock_guard<std::mutex> lock(to_process_mutex);

            json = to_process.front();
            to_process.pop();
        }
        message msg;
        msg.type = json["kind"].get<std::string>();
        if (msg.type == "data") {
            msg.character = json["payload"]["character"].get<std::string>();
            msg.timestamp = json["payload"]["drop"]["timestamp"].get<std::string>();
            for (auto &i : json["payload"]["drop"]["items"].items()) {
                item item;
                item.id = std::stoi(i.key());
                item.quantity = i.value().get<int>();
                msg.drops.push_back(item);
            }
            for (auto &i : json["payload"]["drop"]["curr"].items()) {
                item item;
                item.id = std::stoi(i.key());
                item.quantity = i.value().get<int>();
                msg.currencies.push_back(item);
            }
            std::string out = "character: " + msg.character + " picked up: " + std::to_string(msg.drops.size()) +
                              " items and " + std::to_string(msg.currencies.size()) + " currencies.\nitem list:\n";
            for (auto &i : msg.drops) {
                out += "- " + std::to_string(i.id) + ": " + std::to_string(i.quantity) + "\n";
            }
            for (auto &i : msg.currencies) {
                out += "- " + std::to_string(i.id) + ": " + std::to_string(i.quantity) + "\n";
            }
            api->Log(ELogLevel_DEBUG, addon_name, out.c_str());
        }
    }
}

std::thread process_thread;
void addon_load(AddonAPI *api_p)
{
    api = api_p;

    ImGui::SetCurrentContext(static_cast<ImGuiContext *>(api->ImguiContext));
    ImGui::SetAllocatorFunctions((void *(*)(size_t, void *))(api->ImguiMalloc),
                                 (void (*)(void *, void *))(api->ImguiFree)); // on imgui 1.80+

    api->Renderer.Register(ERenderType_Render, addon_render);
    api->Renderer.Register(ERenderType_OptionsRender, addon_options);
    // api->WndProc.Register(wnd_proc);

    Settings::settings_path = api->Paths.GetAddonDirectory("drf_frontend\\settings.json");
    if (std::filesystem::exists(Settings::settings_path)) {
        Settings::load(Settings::settings_path);
    } /*else {
        Settings::json_settings[Settings::IS_ADDON_ENABLED] = Settings::is_addon_enabled;
        Settings::save(Settings::settings_path);
    }*/
    api->Log(ELogLevel_INFO, addon_name, "addon loaded!");
}

void addon_unload()
{
    api->Log(ELogLevel_INFO, addon_name, "unloading addon...");
    process_thread_running = false;
    if (process_thread.joinable())
        process_thread.join();
    drf_client->close();
    delete drf_client;
    drf_client = nullptr;
    while (!to_process.empty()) {
        to_process.pop();
    }
    api->Renderer.Deregister(addon_render);
    api->Renderer.Deregister(addon_options);
    // api->WndProc.Deregister(wnd_proc);
    api->Log(ELogLevel_INFO, addon_name, "addon unloaded!");
    api = nullptr;
}

void addon_render()
{
    if (drf_client == nullptr && Settings::drf_token.length() == 36) {
        drf_client = new DrfClient();
        drf_client->run();
    }
    if (!process_thread_running) {
        process_thread = std::thread(&process_messages);
        process_thread.detach();
    }
    render_window();
}

void addon_options() { render_options(); }

// UINT wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
//     if (!game_handle)
//         game_handle = hWnd;
//     return uMsg;
// }
