#include "gta/pools.hpp"
#include "gta_util.hpp"
#include "gui.hpp"
#include "pointers.hpp"
#include "views/view.hpp"
#include <array>
#include <format>

namespace big
{
    void view::overlay()
    {
        if (!g.window.ingame_overlay.opened)
            return;

        const bool is_menu_open = g_gui->is_open();
        if (is_menu_open && !g.window.ingame_overlay.show_with_menu_opened)
            return;

        g_gui->push_theme_colors();
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.5f);
        auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                           ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | 
                           ImGuiWindowFlags_NoNav;

        if (!is_menu_open)
            window_flags |= ImGuiWindowFlags_NoMouseInputs;

        if (ImGui::Begin("overlay", nullptr, window_flags))
        {
            std::array<char, 256> buffer;
            auto format_to_ui = [&](const std::string_view fmt_str, auto&&... args) {
                auto result = std::format_to_n(buffer.data(), buffer.size() - 1, fmt_str, std::forward<decltype(args)>(args)...);
                *result.out = '\0';
                ImGui::TextUnformatted(buffer.data());
            };

            ImGui::TextUnformatted("YimMenu");
            ImGui::Separator();

            if (g.window.ingame_overlay.show_fps)
            {
                format_to_ui("{:.0f} {}", ImGui::GetIO().Framerate, "VIEW_OVERLAY_FPS"_T);
            }

            if (g.window.ingame_overlay.show_players)
            {
                if (auto* network_player_mgr = gta_util::get_network_player_mgr(); network_player_mgr != nullptr)
                {
                    format_to_ui("{}: {}/{}", "PLAYERS"_T, network_player_mgr->m_player_count, network_player_mgr->m_player_limit);
                }
            }

            if (g.window.ingame_overlay.show_indicators)
            {
                ImGui::Separator();
                components::overlay_indicator("VIEW_OVERLAY_PLAYER_GODMODE"_T, g.self.god_mode);
                components::overlay_indicator("OFF_THE_RADAR"_T, g.self.off_radar);
                components::overlay_indicator("VIEW_OVERLAY_VEHICLE_GODMODE"_T, g.vehicle.god_mode);
                components::overlay_indicator("NEVER_WANTED"_T, g.self.never_wanted);
                components::overlay_indicator("VIEW_OVERLAY_INFINITE_AMMO"_T, g.weapons.infinite_ammo);
                components::overlay_indicator("VIEW_OVERLAY_ALWAYS_FULL_AMMO"_T, g.weapons.always_full_ammo);
                components::overlay_indicator("VIEW_OVERLAY_INFINITE_MAGAZINE"_T, g.weapons.infinite_mag);
                components::overlay_indicator("VIEW_OVERLAY_AIMBOT"_T, g.weapons.aimbot.enable);
                components::overlay_indicator("VIEW_OVERLAY_TRIGGERBOT"_T, g.weapons.triggerbot);
                components::overlay_indicator("INVISIBILITY"_T, g.self.invisibility);
            }

            if (g.window.ingame_overlay.show_position && g_local_player)
            {
                ImGui::Separator();
                if (auto* pos = g_local_player->get_position(); pos != nullptr)
                {
                    format_to_ui("{}: {:.2f}, {:.2f}, {:.2f}", "VIEW_OVERLAY_POSITION"_T, pos->x, pos->y, pos->z);
                }
            }

            if (g.window.ingame_overlay.show_replay_interface && g_pointers)
            {
                auto& gta = g_pointers->m_gta;
                bool any_pool_valid = (gta.m_ped_pool && *gta.m_ped_pool) || 
                                     (gta.m_vehicle_pool && *gta.m_vehicle_pool && **gta.m_vehicle_pool) || 
                                     (gta.m_prop_pool && *gta.m_prop_pool);

                if (any_pool_valid)
                {
                    ImGui::Separator();
                    if (gta.m_ped_pool && *gta.m_ped_pool)
                    {
                        auto pool = *gta.m_ped_pool;
                        format_to_ui("{}: {}/{}", "VIEW_OVERLAY_PED_POOL"_T, pool->get_item_count(), pool->m_size);
                    }

                    if (gta.m_vehicle_pool && *gta.m_vehicle_pool && **gta.m_vehicle_pool)
                    {
                        auto pool = **gta.m_vehicle_pool;
                        format_to_ui("{}: {}/{}", "VIEW_OVERLAY_VEHICLE_POOL"_T, pool->m_item_count, pool->m_size);
                    }

                    if (gta.m_prop_pool && *gta.m_prop_pool)
                    {
                        auto pool = *gta.m_prop_pool;
                        format_to_ui("{}: {}/{}", "VIEW_OVERLAY_OBJECT_POOL"_T, pool->get_item_count(), pool->m_size);
                    }
                }
            }

            if (g.window.ingame_overlay.show_game_versions && g_pointers)
            {
                ImGui::Separator();
                if (g_pointers->m_gta.m_game_version)
                    format_to_ui("{}: {}", "VIEW_OVERLAY_GAME_VERSION"_T, g_pointers->m_gta.m_game_version);
                
                if (g_pointers->m_gta.m_online_version)
                    format_to_ui("{}: {}", "VIEW_OVERLAY_ONLINE_VERSION"_T, g_pointers->m_gta.m_online_version);
            }
        }
        ImGui::End();
        g_gui->pop_theme_colors();
    }
}
