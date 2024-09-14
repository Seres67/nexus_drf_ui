#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <settings.hpp>

#include <imgui/misc/cpp/imgui_stdlib.h>

bool tmp_open = true;
void render_window()
{
    ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    if (tmp_open && ImGui::Begin("DRF UI##DRFUIMainWindow", &tmp_open, flags)) {
        if (drf_client->is_running()) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected to DRF!");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not connected to DRF!");
        }
        ImGui::End();
    }
}
 
void render_options()
{
    if (ImGui::Checkbox("Enabled##DRFUIEnabled", &Settings::is_addon_enabled)) {
        Settings::json_settings[Settings::IS_ADDON_ENABLED] = Settings::is_addon_enabled;
        Settings::save(Settings::settings_path);
    }
    if (ImGui::InputText("Token##DRFUIToken", &Settings::drf_token, ImGuiInputTextFlags_Password)) {
        Settings::json_settings[Settings::DRF_TOKEN] = Settings::drf_token;
        Settings::save(Settings::settings_path);
    }
}
