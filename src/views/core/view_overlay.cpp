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
            std::array<char, 256> buf;
            ImGui::TextUnformatted("YimMenu");
            ImGui::Separator();

            // FPS
            if (g.window.ingame_overlay.show_fps)
            {
                auto res = std::format_to_n(buf.data(), buf.size() - 1, "{:.0f} {}", ImGui::GetIO().Framerate, "VIEW_OVERLAY_FPS"_T);
                *res.out = '\0';
                ImGui::TextUnformatted(buf.data());
            }

            // Players
            if (g.window.ingame_overlay.show_players)
            {
                if (auto* network_player_mgr = gta_util::get_network_player_mgr(); network_player_mgr != nullptr)
                {
                    auto res = std::format_to_n(buf.data(), buf.size() - 1, "{}: {}/{}", "PLAYERS"_T, network_player_mgr->m_player_count, network_player_mgr->m_player_limit);
                    *res.out = '\0';
                    ImGui::TextUnformatted(buf.data());
                }
            }

            // Indicators
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

            // Position
            if (g.window.ingame_overlay.show_position && g_local_player)
            {
                ImGui::Separator();
                if (auto* pos = g_local_player->get_position(); pos != nullptr)
                {
                    auto res = std::format_to_n(buf.data(), buf.size() - 1, "{}: {:.2f}, {:.2f}, {:.2f}", "VIEW_OVERLAY_POSITION"_T, pos->x, pos->y, pos->z);
                    *res.out = '\0';
                    ImGui::TextUnformatted(buf.data());
                }
            }

            // Replay Interface / Pools
            if (g.window.ingame_overlay.show_replay_interface && g_pointers)
            {
                auto& gta = g_pointers->m_gta;
                bool any_pool = (gta.m_ped_pool && *gta.m_ped_pool) || 
                                (gta.m_vehicle_pool && *gta.m_vehicle_pool && **gta.m_vehicle_pool) || 
                                (gta.m_prop_pool && *gta.m_prop_pool);

                if (any_pool)
                {
                    ImGui::Separator();
                    if (gta.m_ped_pool && *gta.m_ped_pool)
                    {
                        auto p = *gta.m_ped_pool;
                        auto res = std::format_to_n(buf.data(), buf.size() - 1, "{}: {}/{}", "VIEW_OVERLAY_PED_POOL"_T, p->get_item_count(), p->m_size);
                        *res.out = '\0';
                        ImGui::TextUnformatted(buf.data());
                    }

                    if (gta.m_vehicle_pool && *gta.m_vehicle_pool && **gta.m_vehicle_pool)
                    {
                        auto p = **gta.m_vehicle_pool;
                        auto res = std::format_to_n(buf.data(), buf.size() - 1, "{}: {}/{}", "VIEW_OVERLAY_VEHICLE_POOL"_T, p->m_item_count, p->m_size);
                        *res.out = '\0';
                        ImGui::TextUnformatted(buf.data());
                    }

                    if (gta.m_prop_pool && *gta.m_prop_pool)
                    {
                        auto p = *gta.m_prop_pool;
                        auto res = std::format_to_n(buf.data(), buf.size() - 1, "{}: {}/{}", "VIEW_OVERLAY_OBJECT_POOL"_T, p->get_item_count(), p->m_size);
                        *res.out = '\0';
                        ImGui::TextUnformatted(buf.data());
                    }
                }
            }

            // Versions
            if (g.window.ingame_overlay.show_game_versions && g_pointers)
            {
                ImGui::Separator();
                if (g_pointers->m_gta.m_game_version)
                {
                    auto res = std::format_to_n(buf.data(), buf.size() - 1, "{}: {}", "VIEW_OVERLAY_GAME_VERSION"_T, g_pointers->m_gta.m_game_version);
                    *res.out = '\0';
                    ImGui::TextUnformatted(buf.data());
                }
                
                if (g_pointers->m_gta.m_online_version)
                {
                    auto res = std::format_to_n(buf.data(), buf.size() - 1, "{}: {}", "VIEW_OVERLAY_ONLINE_VERSION"_T, g_pointers->m_gta.m_online_version);
                    *res.out = '\0';
                    ImGui::TextUnformatted(buf.data());
                }
            }
        }
        ImGui::End();
        g_gui->pop_theme_colors();
    }
}
