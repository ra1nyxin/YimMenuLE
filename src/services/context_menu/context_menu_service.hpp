#pragma once
#include "backend/command.hpp"
#include "backend/player_command.hpp"
#include "natives.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/ped_animations/ped_animations_service.hpp"
#include "services/vehicle/persist_car_service.hpp"
#include "util/entity.hpp"
#include "util/ped.hpp"
#include "util/teleport.hpp"
#include "util/vehicle.hpp"
#include "util/toxic.hpp"

namespace big
{
	struct context_option
	{
		std::string name;
		std::function<void()> command;
	};

	struct s_context_menu
	{
		ContextEntityType type;
		int current_option = 0;
		ImVec2 menu_size   = {};
		std::vector<context_option> options;
	};

	struct model_bounding_box_screen_space
	{
		ImVec2 edge1, edge2, edge3, edge4;
		ImVec2 edge5, edge6, edge7, edge8;
	};

	class context_menu_service final
	{
	private:
		void fill_model_bounding_box_screen_space();
		static double distance_to_middle_of_screen(const rage::fvector2& screen_pos);

	public:
		context_menu_service();
		~context_menu_service();
		context_menu_service(const context_menu_service&)                = delete;
		context_menu_service(context_menu_service&&) noexcept            = delete;
		context_menu_service& operator=(const context_menu_service&)     = delete;
		context_menu_service& operator=(context_menu_service&&) noexcept = delete;

		bool enabled = false;
		s_context_menu* get_context_menu();
		void get_entity_closest_to_screen_center();
		void load_shared();

		static void disable_control_action_loop();
		static void context_menu();

		Entity m_handle;
		rage::fwEntity* m_pointer{};
		model_bounding_box_screen_space m_model_bounding_box_screen_space;

		s_context_menu vehicle_menu{ContextEntityType::VEHICLE,
		    0,
		    {},
		    {{"修复载具",
		            [this] {
			            if (entity::take_control_of(m_handle))
			            {
				            VEHICLE::SET_VEHICLE_ENGINE_HEALTH(m_handle, 1000.f);
				            VEHICLE::SET_VEHICLE_FIXED(m_handle);
				            VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(m_handle);
				            VEHICLE::SET_VEHICLE_DIRT_LEVEL(m_handle, 0.f);
			            }
		            }},
				{"设为无敌",
				    [this] {
				        if (entity::take_control_of(m_handle))
				        {
				            ENTITY::SET_ENTITY_INVINCIBLE(m_handle, true);
				            ENTITY::SET_ENTITY_PROOFS(m_handle, true, true, true, true, true, true, true, true);
				        }
				    }},
				{"性能改装",
					[this] {
						if (!ENTITY::DOES_ENTITY_EXIST(m_handle) || !ENTITY::IS_ENTITY_A_VEHICLE(m_handle))
							return;

						if (entity::take_control_of(m_handle))
						{
							if (VEHICLE::GET_NUM_MOD_KITS(m_handle) > 0)
							{
								VEHICLE::SET_VEHICLE_MOD_KIT(m_handle, 0);
							}
							int performance_mods[] = { 11, 12, 13, 15, 16 }; 
							for (int mod_type : performance_mods)
							{
								int max_mod = VEHICLE::GET_NUM_VEHICLE_MODS(m_handle, mod_type);
								if (max_mod > 0)
								{
									VEHICLE::SET_VEHICLE_MOD(m_handle, mod_type, max_mod - 1, false);
								}
							}
							VEHICLE::TOGGLE_VEHICLE_MOD(m_handle, 18, true);
							VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(m_handle, false);
							ENTITY::SET_ENTITY_AS_MISSION_ENTITY(m_handle, true, true);
						}
					}},
				{"摧毁引擎",
		         [this] {
			         if (entity::take_control_of(m_handle))
			         {
				         VEHICLE::SET_VEHICLE_ENGINE_HEALTH(m_handle, -4000.f);
				         VEHICLE::SET_VEHICLE_ENGINE_ON(m_handle, false, true, false);
			         }
		         }},
		        {"爆胎",
		            [this] {
			            if (entity::take_control_of(m_handle))
			            {
				            VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(m_handle, true);
				            for (int i = 0; i < 8; i++)
					            VEHICLE::SET_VEHICLE_TYRE_BURST(m_handle, i, true, 1000.0);
			            }
		            }},
		        {"弹射起飞",
		            [this] {
			            if (entity::take_control_of(m_handle))
				            ENTITY::APPLY_FORCE_TO_ENTITY(m_handle, 1, 0.f, 0.f, 100.f, 0.f, 0.f, 0.f, 0, 0, 1, 1, 0, 1);
		            }},
				{"重力异常",
					[this] {
						if (entity::take_control_of(m_handle))
							VEHICLE::SET_VEHICLE_GRAVITY(m_handle, false);
					}},
				{"锁定车门",
					[this] {
						if (entity::take_control_of(m_handle))
							VEHICLE::SET_VEHICLE_DOORS_LOCKED(m_handle, 4);
					}},
		        {"传到导航点",
		            [this] {
			            Blip waypoint = HUD::GET_FIRST_BLIP_INFO_ID(8);
			            if (HUD::DOES_BLIP_EXIST(waypoint))
			            {
				            if (entity::take_control_of(m_handle))
				            {
					            Vector3 coords = HUD::GET_BLIP_INFO_ID_COORD(waypoint);
					            ENTITY::SET_ENTITY_COORDS(m_handle, coords.x, coords.y, coords.z, false, false, false, false);
					            VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(m_handle, 5.f);
				            }
			            }
		            }},
		        {"踢出驾驶员",
		            [this] {
						Ped driver = VEHICLE::GET_PED_IN_VEHICLE_SEAT(m_handle, -1, 0);
						if (ENTITY::DOES_ENTITY_EXIST(driver)) {
							if (ped::get_player_from_ped(driver) != NULL)
							{
								static player_command* command = player_command::get("vehkick"_J);
								command->call(ped::get_player_from_ped(driver), {});
							}
							TASK::CLEAR_PED_TASKS_IMMEDIATELY(driver);
						}
		            }},
		        {"坐进载具", [this] {
			         teleport::into_vehicle(m_handle);
		         }}}};

		s_context_menu ped_menu{ContextEntityType::PED,
		    0,
		    {},
		    {{"招募保镖", [this] {
			         TASK::CLEAR_PED_TASKS(m_handle);
			         PED::SET_PED_AS_GROUP_MEMBER(m_handle, PED::GET_PED_GROUP_INDEX(self::ped));
			         PED::SET_PED_RELATIONSHIP_GROUP_HASH(m_handle, PED::GET_PED_RELATIONSHIP_GROUP_HASH(self::ped));
			         PED::SET_PED_NEVER_LEAVES_GROUP(m_handle, true);
			         PED::SET_PED_COMBAT_ABILITY(m_handle, 2);
			         WEAPON::GIVE_WEAPON_TO_PED(m_handle, "weapon_specialcarbine"_J, 9999, false, true);
		         }},
				{"意志控制", [this] {
					if (!ENTITY::DOES_ENTITY_EXIST(m_handle) || ENTITY::IS_ENTITY_DEAD(m_handle, 0))
						return;

					if (entity::take_control_of(m_handle)) {
						TASK::CLEAR_PED_TASKS_IMMEDIATELY(m_handle);
						WEAPON::GIVE_WEAPON_TO_PED(m_handle, "WEAPON_CARBINERIFLE"_J, 999, false, true);
						PED::SET_PED_COMBAT_ATTRIBUTES(m_handle, 46, true);
						PED::SET_PED_COMBAT_ATTRIBUTES(m_handle, 5, true);
						PED::SET_PED_COMBAT_ABILITY(m_handle, 2);
						PED::SET_PED_RELATIONSHIP_GROUP_HASH(m_handle, "HATES_PLAYER"_J);
						PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(m_handle, true);
						TASK::TASK_COMBAT_HATED_TARGETS_AROUND_PED(m_handle, 500.f, 0);
						TASK::TASK_AGGRESSIVE_CLEAN_SET_OF_NODES(m_handle, 0, 0, 0); 
					}
				}},
		        {"杀死",
		            [this] {
			            ped::kill_ped(m_handle);
		            }},
				{"克隆",
					[this] {
						PED::CLONE_PED(m_handle, TRUE, TRUE, TRUE);
					}},
		        {"布娃娃模式",
		            [this] {
			            PED::SET_PED_TO_RAGDOLL(m_handle, 5000, 5000, 0, 0, 0, 0);
		            }},
				{"焚化", [this] {
					if (entity::take_control_of(m_handle)) {
						FIRE::START_ENTITY_FIRE(m_handle);
					}
				}},
		        {"播放动画",
		            [this] {
			            if (STREAMING::DOES_ANIM_DICT_EXIST(g_ped_animation_service.current_animation.dict.data()))
				            g_ped_animation_service.play_saved_ped_animation(g_ped_animation_service.current_animation, m_handle);
			            else
				            ped::ped_play_animation(m_handle, "mini@strip_club@private_dance@part1", "priv_dance_p1", 3.5f, -4.0f, -1, 1);
		            }}}};

		s_context_menu object_menu{ContextEntityType::OBJECT, 0, {}, {
			{"锁定位置", [this] {
				if (entity::take_control_of(m_handle))
					ENTITY::FREEZE_ENTITY_POSITION(m_handle, true);
			}},
			{"解除锁定", [this] {
				if (entity::take_control_of(m_handle))
					ENTITY::FREEZE_ENTITY_POSITION(m_handle, false);
			}},
			{"隐身", [this] {
				if (entity::take_control_of(m_handle))
					ENTITY::SET_ENTITY_VISIBLE(m_handle, false, false);
			}},
			{"显示", [this] {
				if (entity::take_control_of(m_handle))
					ENTITY::SET_ENTITY_VISIBLE(m_handle, true, false);
			}},
			{"标记持久化", [this] {
				if (entity::take_control_of(m_handle))
					ENTITY::SET_ENTITY_AS_MISSION_ENTITY(m_handle, true, true);
			}}
		}};

		s_context_menu player_menu{ContextEntityType::PLAYER,
		    0,
		    {},
		    {{"选中玩家",
		         [this] {
			         g_player_service->set_selected(ped::get_player_from_ped(m_handle));
		         }},
				{"回血回甲",
					[this] {
						if (entity::take_control_of(m_handle)) {
							ENTITY::SET_ENTITY_HEALTH(m_handle, ENTITY::GET_ENTITY_MAX_HEALTH(m_handle), 0, 0);
							PED::ADD_ARMOUR_TO_PED(m_handle, 100);
						}
					}},
				{"给予武器", [this] {
					auto player = ped::get_player_from_ped(m_handle);
					if (player) {
						static player_command* command = player_command::get("giveallweaps"_J);
						command->call(player, {});
					}
				}},
		        {"偷取服装",
		            [this] {
			            ped::steal_outfit(m_handle);
		            }},
		        {"踢出战局",
		            [this] {
			            static player_command* command = player_command::get("smartkick"_J);
			            command->call(ped::get_player_from_ped(m_handle), {});
		            }},
		        {"强制缴械",
		            [this] {
			            static player_command* command = player_command::get("remweaps"_J);
			            command->call(ped::get_player_from_ped(m_handle), {});
		            }},
		        {"布娃娃模式", [this] {
			         static player_command* command = player_command::get("ragdoll"_J);
			         command->call(ped::get_player_from_ped(m_handle), {});
		         }}}};

		s_context_menu shared_menu{ContextEntityType::SHARED,
		    0,
		    {},
		    {{"复制 HASH",
		         [this] {
			         ImGui::SetClipboardText(std::format("0x{:08X}", (rage::joaat_t)m_pointer->m_model_info->m_hash).c_str());
		         }},
		        {"爆炸",
		            [this] {
			            rage::fvector3 pos = *m_pointer->m_navigation->get_position();
			            FIRE::ADD_EXPLOSION(pos.x, pos.y, pos.z, 1, 1000, 1, 0, 1, 0);
		            }},
		        {"拉到身边",
		            [this] {
			            rage::fvector3 pos = *g_local_player->m_navigation->get_position();
			            if (PED::IS_PED_A_PLAYER(m_handle))
			            {
				            if (auto plyr = g_player_service->get_by_id(NETWORK::NETWORK_GET_PLAYER_INDEX_FROM_PED(m_handle)))
					            teleport::teleport_player_to_coords(plyr, {pos.x, pos.y, pos.z});
			            }
			            else if (entity::take_control_of(m_handle))
				            ENTITY::SET_ENTITY_COORDS(m_handle, pos.x, pos.y, pos.z, false, false, false, false);
		            }},
		        {"删除", [this] {
			         if (entity::take_control_of(m_handle))
				         entity::delete_entity(m_handle);
		         }}}};
		std::unordered_map<ContextEntityType, s_context_menu> options = {{ContextEntityType::VEHICLE, vehicle_menu}, {ContextEntityType::PLAYER, player_menu}, {ContextEntityType::PED, ped_menu}, {ContextEntityType::SHARED, shared_menu}, {ContextEntityType::OBJECT, object_menu}};
	};
	inline context_menu_service* g_context_menu_service{};
}
