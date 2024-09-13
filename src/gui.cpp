#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <settings.hpp>

bool tmp_open = true;
void render_window()
{
    ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    if (tmp_open && ImGui::Begin("Template##TemplateMainWindow", &tmp_open, flags)) {
        ImGui::Text("Hello from template!");
        ImGui::End();
    }
}
 
void render_options()
{
    if (ImGui::Checkbox("Enabled##TemplateEnabled", &Settings::is_addon_enabled)) {
        Settings::json_settings[Settings::IS_ADDON_ENABLED] = Settings::is_addon_enabled;
        Settings::save(Settings::settings_path);
    }
}
