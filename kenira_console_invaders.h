#include <kenira_console.h>
#include <map>


// TODO:
// - ECS
// - replay level buttons
// - buttons to continue after game won / reset after lost
// - menu tree (map)
// - type and current_level are same
// - better level advance system
// - consolidate drawing text and spawning menu item, just change the collision
// - flexible positions for menu items, depending on window size
// - check for min, max values for upgrades (not even spawn upgrades?)
// - fixed upgrade points per level, you can redo a level (risk of losing live vs. chance to get more upgrades)
// - unlock shield after achieving something (finish level 2 without damage? finish level 3 in general?)
// - characters: gives you quests like finish level 2 without taking damage for shield. permanent 2 guns upgrade? different gun, machinegun no precharging but higher fire rate
// - collision detection between what and what? projectiles and players, projectiles and aliens, players and aliens. so, every category with every other
// (- better game loop with constant internal calculation but variable fps)
  // - upgrades for firing a 2 damage projectile every X shots, precharging more shots, faster precharging shots, shooting 2 projectiles every X shots
	// - if pos_proj.x AND pos_proj.x + proj.width either both smaller or bigger as either bounds of target, then no collision. for bigger projectiles
	// - upgrade system
	// - have position velocity of Entity. then only separate position in box which is relative to Entity position
//
//

namespace kenira
{
namespace consoleinv
{

constexpr int CI_TYPE_PROJECTILE = 2;
constexpr int CI_TYPE_TEXT = 5;
constexpr int CI_TYPE_ND = 9;
constexpr int CI_TYPE_PLAYER = 100;
constexpr int CI_TYPE_ALIEN_BASIC = 200;		// this and greater for alien types (so you can check for type >= _BASIC_ALIEN to see if it's an enemy)
constexpr int CI_TYPE_ALIEN_ADVANCED = 201;
constexpr int CI_TYPE_ALIEN_ELITE = 202;

constexpr int CI_STATE_ND = 10;
constexpr int CI_STATE_WON = 11;
constexpr int CI_STATE_LOST = 12;
constexpr int CI_STATE_QUIT = 13;
constexpr int CI_STATE_COMPLETE = 14;

constexpr int CI_UPMENU_HEALTH = 30;
constexpr int CI_UPMENU_SHOT_DELAY = 31;
constexpr int CI_UPMENU_DMG = 32;
constexpr int CI_UPMENU_DMG_CHANCE = 33;
constexpr int CI_UPMENU_DUPLICATE_CHANCE = 34;
constexpr int CI_UPMENU_CHARGE_DELAY = 35;
constexpr int CI_UPMENU_CHARGE_MAX = 36;
constexpr int CI_SPEEDMENU_1 = 37;
constexpr int CI_SPEEDMENU_2 = 38;
constexpr int CI_SPEEDMENU_3 = 39;
constexpr int CI_MAINMENU_SPEED = 40;
constexpr int CI_MAINMENU_UP = 41;
constexpr int CI_MAINMENU_EXIT = 42;
constexpr int CI_MENU_FIGHT = 43;
constexpr int CI_MENU_TO_MAIN = 44;
constexpr int CI_MENU_RESTART_GAME = 45;




constexpr int CI_LEVEL_MAINMENU = 50;
constexpr int CI_LEVEL_SPEEDMENU = 51;
constexpr int CI_LEVEL_UPMENU = 52;
constexpr int CI_LEVEL_GAMEWON = 58;
constexpr int CI_LEVEL_GAMEOVER = 59;
constexpr int CI_LEVEL_01 = 80;					// level type >= CI_LEVEL_01 will be used as check if level is regular fighting level with enemies
constexpr int CI_LEVEL_02 = 81;
constexpr int CI_LEVEL_03 = 82;
constexpr int CI_LEVEL_04 = 83;
constexpr int CI_LEVEL_05 = 84;

constexpr int HEALTH_PLAYER_DEFAULT = 5;
constexpr int UPGRADE_SHOOTING_DELAY_MAX = 2;
constexpr int UPGRADE_PRECHARGE_DELAY_MAX = 2;



std::chrono::high_resolution_clock::time_point time_last_escape;
constexpr int64_t time_delay_escape_ms = 500;


struct Collision_Box
{
	bool collision_disabled = false;

	maths::Vec2i pos_rel;
	maths::Vec2i dim;

	Collision_Box()
	{
	};

	Collision_Box(maths::Vec2i _pos_rel, maths::Vec2i _dim)
	{
		pos_rel = _pos_rel;
		dim = _dim;
	}
};

struct Entity
{
public:
	Collision_Box box;

	//int id = -1;
	int health = 1;
	int max_health = 1;
	int type = CI_TYPE_ND;
	int type_parent = CI_TYPE_ND;
	std::vector<std::string> visual;
	int char_info_attribute = 7;
	maths::Vec2i dimensions;

	int shots_precharged = 0;
	int shots_precharged_max = 0;
	int precharge_delay = -1;
	bool shooting_queued = false;
	int tick_last_shot = 0;
	int shoot_delay = 7;
	int collision_damage = 1;
	int shot_damage_bonus = 0;
	int shot_damage_bonus_chance = 0;
	int shot_duplicate_chance = 0;

	types::time_point time_created;
	int life_span_ms = -1;

	maths::Vec2i pos;
	maths::Vec2i vel;
	maths::Vec2i projectile_spawn_offset;
	maths::Vec2i projectile_spawn_vel;

	Entity(maths::Vec2i _pos, maths::Vec2i _vel, int _type, int _type_parent)
		//Entity(maths::Vec2i _pos, maths::Vec2i _vel, int _type, types::vstr _visual, EntityTemplate entity_template, maths::Vec2i _spawn_offset, maths::Vec2i _spawn_velocity)
	{
		//pos = _pos;
		//vel = _vel;
		type = _type;
		type_parent = _type_parent;
		health = 1;
		time_created = std::chrono::high_resolution_clock::now();
		
		switch (_type)
		{
		case CI_TYPE_TEXT:
		{
			visual = { "" };
			box.collision_disabled = true;
			health = 1;
			shoot_delay = 4;
			precharge_delay = shoot_delay;
			shots_precharged_max = 3;
			collision_damage = 0;
			char_info_attribute = 0x0f;
			projectile_spawn_offset = { 0, 0 };
			projectile_spawn_vel = { 0, 0 };
			break;
		}
		case CI_TYPE_PLAYER:
		{
			visual = { "/A\\" , "AXA" };
			health = HEALTH_PLAYER_DEFAULT;
			shoot_delay = 5;
			precharge_delay = shoot_delay;
			shots_precharged_max = 3;
			collision_damage = 0;
			char_info_attribute = 7;
			projectile_spawn_offset = { 1, -2 };
			projectile_spawn_vel = { 0, -1 };
			break;
		}
		case CI_TYPE_ALIEN_BASIC:
		{
			visual = { "###", "\\|/" };
			health = 2;
			shoot_delay = 7;
			precharge_delay = -1;
			shots_precharged_max = 0;
			collision_damage = 1;
			projectile_spawn_offset = { 1, 3 };
			projectile_spawn_vel = { 0, 1 };
			char_info_attribute = 0x0002; //0x0a
			break;
		}
		case CI_TYPE_ALIEN_ADVANCED:
		{
			visual = { "XXX", "\\|/" };
			health = 3;
			shoot_delay = 4;
			precharge_delay = -1;
			shots_precharged_max = 0;
			collision_damage = 1;
			projectile_spawn_offset = { 1, 3 };
			projectile_spawn_vel = { 0, 1 };
			char_info_attribute = 0x000a; //0x0a
			break;
		}
		case CI_TYPE_ALIEN_ELITE:
		{
			visual = { "TTT", "\\|/" };
			health = 6;
			shoot_delay = 5;
			precharge_delay = -1;
			shots_precharged_max = 0;
			collision_damage = 1;
			projectile_spawn_offset = { 1, 3 };
			projectile_spawn_vel = { 0, 1 };
			char_info_attribute = 0x000f; //0x0a
			break;
		}
		case CI_TYPE_PROJECTILE:
		{
			visual = { "o" };
			health = 1;
			shoot_delay = -1;
			precharge_delay = -1;
			shots_precharged_max = 0;
			collision_damage = 1;

			switch (_type_parent)
			{
			case CI_TYPE_PLAYER:
			{
				char_info_attribute = 0x000b;
				break;
			}
			case CI_TYPE_ALIEN_BASIC:
			{
				char_info_attribute = 0x000c;
				break;
			}
			case CI_TYPE_ALIEN_ADVANCED:
			{
				char_info_attribute = 0x000c;
				break;
			}
			case CI_TYPE_ALIEN_ELITE:
			{
				collision_damage = 3;
				char_info_attribute = 0x000d;
				break;
			}
			}

			projectile_spawn_offset = { 0, 0 };
			projectile_spawn_vel = { 0, 0 };
			break;
		}

		}


		max_health = health;
		dimensions.x = kenira::io::get_max_string_length_in_vstr(visual);
		dimensions.y = (int)visual.size();

		pos = _pos;
		vel = _vel;
		box.dim.x = dimensions.x - 1;
		box.dim.y = dimensions.y - 1;
	}
};

struct Upgrades
{
	int health;
	int shot_delay;
	int shot_damage;
	int shot_damage_chance;
	int shot_duplicate_chance;
	int precharge_max;
	int precharge_delay;

	Upgrades()
	{
		health = 0;
		shot_delay = 0;
		shot_damage = 0;
		shot_damage_chance = 0;
		shot_duplicate_chance = 0;
		precharge_max = 0;
		precharge_delay = 0;
	}

	Upgrades& operator+=(const Upgrades& right)
	{
		this->health += right.health;
		this->precharge_delay += right.precharge_delay;
		this->precharge_max += right.precharge_max;
		this->shot_damage += right.shot_damage;
		this->shot_damage_chance += right.shot_damage_chance;
		this->shot_delay += right.shot_delay;
		this->shot_duplicate_chance += right.shot_duplicate_chance;
		return *this;
	}

	Upgrades operator+(const Upgrades& right) const
	{
		Upgrades temp;
		temp.health = this->health + right.health;
		temp.precharge_delay = this->precharge_delay + right.precharge_delay;
		temp.precharge_max = this->precharge_max + right.precharge_max;
		temp.shot_damage = this->shot_damage + right.shot_damage;
		temp.shot_damage_chance = this->shot_damage_chance + right.shot_damage_chance;
		temp.shot_delay = this->shot_delay + right.shot_delay;
		temp.shot_duplicate_chance = this->shot_duplicate_chance + right.shot_duplicate_chance;
		return temp;
	}

	void reset()
	{
		health = 0;
		shot_delay = 0;
		shot_damage = 1;
		shot_damage_chance = 0;
		shot_duplicate_chance = 0;
		precharge_max = 0;
		precharge_delay = 0;
	}
};

struct Spawn
{
	maths::Vec2i pos;
	maths::Vec2i vel;
	int type;
};

struct Menu_Entity : public Entity
{
public:
	Upgrades effect;
	int effect_int;

	int cost;			// for upgrademenu items
	int time_interval;

	Menu_Entity(maths::Vec2i _pos, maths::Vec2i _vel, int _type, int _type_parent) : Entity(_pos, _vel, _type, _type_parent)
	{
		health = 1;
		max_health = health;
		shoot_delay = -1;
		precharge_delay = -1;
		shots_precharged_max = 0;
		collision_damage = 0;
		projectile_spawn_offset = { 1, 3 };
		projectile_spawn_vel = { 0, 1 };
		char_info_attribute = 0x0007;

		switch (_type)
		{
		case CI_MAINMENU_SPEED:
		{
			effect_int = CI_LEVEL_SPEEDMENU;
			visual = { "SET SPEED" };
			break;
		}
		case CI_MAINMENU_UP:
		{
			effect_int = CI_LEVEL_UPMENU;
			visual = { "UPGRADE MENU" };
			break;
		}
		case CI_MAINMENU_EXIT:
		{
			effect_int = CI_STATE_QUIT;
			visual = { "EXIT" };
			char_info_attribute = 0x000f;
			break;
		}
		case CI_MENU_FIGHT:
		{
			effect_int = CI_MENU_FIGHT;
			visual = { "FIGHT!" };
			break;
		}
		case CI_MENU_TO_MAIN:
		{
			effect_int = CI_MENU_TO_MAIN;
			visual = { "BACK" };
			char_info_attribute = 0x000f;
			break;
		}
		case CI_MENU_RESTART_GAME:
		{
			effect_int = CI_MENU_RESTART_GAME;
			visual = { "MAIN MENU" };
			char_info_attribute = 0x000f;
			break;
		}
		case CI_UPMENU_HEALTH:
		{
			cost = 2;
			effect.health = 1;
			visual = { "HEALTH", "", "+1", "COST " + std::to_string(cost) };
			break;
		}
		case CI_UPMENU_SHOT_DELAY:
		{
			cost = 10;
			effect.shot_delay = -1;
			visual = { "SHOOTING", "SPEED", "+1", "COST " + std::to_string(cost) };
			break;
		}
		case CI_UPMENU_DMG:
		{
			cost = 4;
			effect.shot_damage = 1;
			visual = { "CRIT", "DAMAGE", "+1", "COST " + std::to_string(cost) };
			break;
		}
		case CI_UPMENU_DMG_CHANCE:
		{
			cost = 2;
			effect.shot_damage_chance = 5;
			visual = { "CRIT", "CHANCE", "+5%", "COST " + std::to_string(cost) };
			break;
		}
		case CI_UPMENU_DUPLICATE_CHANCE:
		{
			cost = 3;
			effect.shot_duplicate_chance = 5;
			visual = { "DOUBLESHOT", "CHANCE", "+5%", "COST " + std::to_string(cost) };
			break;
		}
		case CI_UPMENU_CHARGE_DELAY:
		{
			cost = 4;
			effect.precharge_delay = -1;
			visual = { "CHARGE", "DELAY", "-1 TICK", "COST " + std::to_string(cost) };
			break;
		}
		case CI_UPMENU_CHARGE_MAX:
		{
			cost = 3;
			effect.precharge_max = 1;
			visual = { "MAXIMUM", "CHARGES", "+1", "COST " + std::to_string(cost) };
			break;
		}
		case CI_SPEEDMENU_1:
		{
			time_interval = 256;
			visual = { "SLOW" };
			break;
		}
		case CI_SPEEDMENU_2:
		{
			time_interval = 128;
			visual = { "MEDIUM" };
			break;
		}
		case CI_SPEEDMENU_3:
		{
			time_interval = 64;
			visual = { "FAST" };
			break;
		}

		}

		max_health = health;
		dimensions.x = kenira::io::get_max_string_length_in_vstr(visual);
		dimensions.y = (int)visual.size();

		box.dim.x = dimensions.x - 1;
		box.dim.y = dimensions.y - 1;
	}
};

class Console_Invaders
{
	friend class Level;
private:
	Upgrades upgrades;

	const std::array<int, 2> win_size = { 80, 40 };
	std::map<int, int> upgrade_points_per_level;

	int state = CI_TYPE_ND;
	int lives = 3;
	int lives_start = 3;
	int current_level = CI_LEVEL_MAINMENU;
	int next_fighting_level = CI_LEVEL_01;
	int upgrade_points = 0;

	std::chrono::high_resolution_clock::time_point time_start, time_last_escape;
	
	int time_interval_ms = 128;

	maths::Vec2i dim;

public:
	int run();

	void initialize_game();
	void advance_level();
	void update_state();
	void draw_text_centered(const types::vstr& vstr);
};

class Level
{
public:
	std::vector<Entity> entities;
	std::vector<Menu_Entity> menu_entities;
	std::set<int> indices_menu_entities_shot;
	int menu_shoot_delay = 15;
	
	Upgrades& r_upgrades;
	int& r_upgrade_points;
	int& r_lives;
	int& r_time_interval_ms;
	int& r_current_level;
	std::map<int, int>& r_upgrade_points_by_level;


	int type;
	int level_state = CI_STATE_ND;
	int tick = 0;
	int tick_ignore_quit_until = 0;
	int speed_change_delay_ms = 500;
	int time_interval_max = 256;
	int time_interval_min = 16;
	int time_interval_ms_menu = 32;
	bool menu_mode = false;

	maths::Vec2i dim;	// screen dimensions
	int border_left = 2; 
	int border_right = 2;
	int border_top = 0;
	int border_bottom = 0;
	maths::Vec2i player_start_pos_default;

	std::chrono::high_resolution_clock::time_point time_start, time_last_tick, time_last_speed_change;


	/*Level(int& _current_level, Upgrades& _upgrades, int& _upgrade_points, std::map<int, int>& _upgrade_points_by_level, int& _lives, int& _time_interval_ms, maths::Vec2i _dimensions) :
		r_current_level(_current_level),
		r_upgrades(_upgrades),
		r_upgrade_points(_upgrade_points),
		r_upgrade_points_by_level(_upgrade_points_by_level,
		r_lives(_lives),
		r_time_interval_ms(_time_interval_ms))*/
	Level(Console_Invaders* p_console_invaders, maths::Vec2i _dimensions) :
		r_current_level(p_console_invaders->current_level),
		r_upgrades(p_console_invaders->upgrades),
		r_upgrade_points(p_console_invaders->upgrade_points),
		r_upgrade_points_by_level(p_console_invaders->upgrade_points_per_level),
		r_lives(p_console_invaders->lives),
		r_time_interval_ms(p_console_invaders->time_interval_ms)
	{
		entities.clear();
		menu_entities.clear();
		dim = _dimensions;
		player_start_pos_default = { dim.x / 2, dim.y - 1 };

		type = r_current_level;

		switch (type)
		{
		case CI_LEVEL_MAINMENU:
		{
			spawn_player(player_start_pos_default, true);
			menu_mode = true;

			draw_text("'A' and 'D' to move, 'SPACE' to shoot", { 2,2 });

			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 10, 7 }, maths::Vec2i{ 0, 0 }, CI_MENU_FIGHT, CI_LEVEL_MAINMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 25, 7 }, maths::Vec2i{ 0, 0 }, CI_MAINMENU_UP, CI_LEVEL_MAINMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 47, 7 }, maths::Vec2i{ 0, 0 }, CI_MAINMENU_SPEED, CI_LEVEL_MAINMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 67, 7 }, maths::Vec2i{ 0, 0 }, CI_MAINMENU_EXIT, CI_LEVEL_MAINMENU));
			
			break;
		}
		case CI_LEVEL_SPEEDMENU:
		{
			spawn_player(player_start_pos_default, true);
			menu_mode = true;

			//"Select speed: 1 slow, 2 normal, 3 fast"
			draw_text("(Press + or - during the game to speed up or slow down the game)", { 2,2 });
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 12, 7 }, maths::Vec2i{ 0, 0 }, CI_SPEEDMENU_1, CI_LEVEL_SPEEDMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 32, 7 }, maths::Vec2i{ 0, 0 }, CI_SPEEDMENU_2, CI_LEVEL_SPEEDMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 52, 7 }, maths::Vec2i{ 0, 0 }, CI_SPEEDMENU_3, CI_LEVEL_SPEEDMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 67, 7 }, maths::Vec2i{ 0, 0 }, CI_MENU_TO_MAIN, CI_LEVEL_SPEEDMENU));
			
			break;
		}
		case CI_LEVEL_UPMENU:
		{
			spawn_player(player_start_pos_default, true);
			menu_mode = true;

			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 02, 7 }, maths::Vec2i{ 0, 0 }, CI_UPMENU_HEALTH, CI_LEVEL_UPMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 11, 7 }, maths::Vec2i{ 0, 0 }, CI_UPMENU_SHOT_DELAY, CI_LEVEL_UPMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 21, 7 }, maths::Vec2i{ 0, 0 }, CI_UPMENU_DMG, CI_LEVEL_UPMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 31, 7 }, maths::Vec2i{ 0, 0 }, CI_UPMENU_DMG_CHANCE, CI_LEVEL_UPMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 40, 7 }, maths::Vec2i{ 0, 0 }, CI_UPMENU_DUPLICATE_CHANCE, CI_LEVEL_UPMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 52, 7 }, maths::Vec2i{ 0, 0 }, CI_UPMENU_CHARGE_DELAY, CI_LEVEL_UPMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 62, 7 }, maths::Vec2i{ 0, 0 }, CI_UPMENU_CHARGE_MAX, CI_LEVEL_UPMENU));
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 73, 7 }, maths::Vec2i{ 0, 0 }, CI_MENU_TO_MAIN, CI_LEVEL_UPMENU));
			break;
		}
		case CI_LEVEL_GAMEWON:
		{
			menu_mode = true;

			draw_text("YOU GENDER NEUTRAL CHILD OF A PROJECTILE DISCHARGER, YOU REALLY DID IT!", { 2, 15 });
			kenira::io::wait_ms(3000);

			spawn_player(player_start_pos_default, true);
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 30, 25 }, maths::Vec2i{ 0, 0 }, CI_MENU_TO_MAIN, CI_LEVEL_GAMEWON));
			menu_entities.back().visual = { "MAIN MENU" };

			break;
		}
		case CI_LEVEL_GAMEOVER:
		{
			menu_mode = true;

			draw_text("WOWZERS, YOU REALLY MESSED UP THERE! THAT'S GAME OVER FOR YOU", { 5, 15 });
			kenira::io::wait_ms(3000);
			
			spawn_player(player_start_pos_default, true);
			menu_entities.push_back(Menu_Entity(maths::Vec2i{ 30, 25 }, maths::Vec2i{ 0, 0 }, CI_MENU_RESTART_GAME, CI_LEVEL_GAMEOVER));
			menu_entities.back().visual = {"MAIN MENU"};

			break;
		}
		case CI_LEVEL_01:
		{
			spawn_player(player_start_pos_default);
			for (int i = 0; i < 12; ++i)
			{
				entities.push_back(Entity({ 2 + 5 * i, 4 }, { 1,0 }, CI_TYPE_ALIEN_BASIC, CI_TYPE_ND));
			}
			break;
		}
		case CI_LEVEL_02:
		{
			spawn_player(player_start_pos_default);
			for (int i = 0; i < 7; ++i)
			{
				entities.push_back(Entity({ 2 + 10 * i, 4 }, { 1,0 }, CI_TYPE_ALIEN_BASIC, CI_TYPE_ND));
			}
			for (int i = 0; i < 6; ++i)
			{
				entities.push_back(Entity({ 7 + 10 * i, 4 }, { 1,0 }, CI_TYPE_ALIEN_ADVANCED, CI_TYPE_ND));
			}
			break;
		}
		case CI_LEVEL_03:
		{
			spawn_player(player_start_pos_default);
			types::vint locations_basic = { 2, 12, 47, 57 };
			types::vint locations_advanced = { 7, 27, 32, 52 };
			types::vint locations_elite = { 17, 22, 37, 42 };

			for (auto&& x : locations_basic)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_BASIC, CI_TYPE_ND));
			}
			for (auto&& x : locations_advanced)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_ADVANCED, CI_TYPE_ND));
			}
			for (auto&& x : locations_elite)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_ELITE, CI_TYPE_ND));
			}
			break;
		}
		case CI_LEVEL_04:
		{
			spawn_player(player_start_pos_default);
			types::vint locations_basic = {};
			types::vint locations_advanced = { 2, 27, 52 };
			types::vint locations_elite = { 7, 12, 17, 22, 32, 37, 42, 47 };

			for (auto&& x : locations_basic)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_BASIC, CI_TYPE_ND));
			}
			for (auto&& x : locations_advanced)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_ADVANCED, CI_TYPE_ND));
			}
			for (auto&& x : locations_elite)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_ELITE, CI_TYPE_ND));
			}
			break;
		}
		case CI_LEVEL_05:
		{
			spawn_player(player_start_pos_default);
			types::vint locations_basic = {};
			types::vint locations_advanced = {};
			types::vint locations_elite = { 2, 7, 12, 17, 22, 27, 32, 37, 42, 47, 52, 57, 62 };

			for (auto&& x : locations_basic)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_BASIC, CI_TYPE_ND));
			}
			for (auto&& x : locations_advanced)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_ADVANCED, CI_TYPE_ND));
			}
			for (auto&& x : locations_elite)
			{
				entities.push_back(Entity({ x, 4 }, { 1,0 }, CI_TYPE_ALIEN_ELITE, CI_TYPE_ND));
			}
			break;
		}
		}
	}


	int run();


	void initialize();

	int get_player_index();
	void get_keys_and_wait();
	void move();
	bool collision(const Entity& one, const Entity& two);
	void collision_detection();
	void delete_dead_entities();
	void check_if_menu_shot();
	void spawn_player(maths::Vec2i _pos, bool menu_mode = false);
	void spawn_entities();
	void spawn_projectile(int index_parent);
	void shoot();
	void draw();
	void draw_text(const std::string& str, maths::Vec2i _pos, int duration_ms = -1);
	void draw_text(const std::vector<std::string>& vstr, maths::Vec2i _pos, int duration_ms = -1);
	void draw_stat(int stat, std::string _stat_name, std::string unit_symbol, maths::Vec2i _pos, int attribute = 7);
	void draw_stat(int stat, std::string _stat_name, maths::Vec2i _pos, int attribute = 7);
	void draw_text(const std::string& str);
	void draw_text(const types::vstr& vstr);
	void draw_text_centered(const std::string& str);
	void draw_text_centered(const types::vstr& vstr);
	bool is_key_pressed(int key);
	bool change_speed(double factor);
	void update_state();
};




int Console_Invaders::run()
{
	initialize_game();

	while (state != CI_STATE_QUIT)
	{
		lives = lives_start;
		while (state == CI_STATE_ND)
		{
			Level level(this, dim);
			
			level.initialize();
			int ret = level.run();

			update_state();

			if (ret == CI_STATE_WON || ret == CI_STATE_COMPLETE)
			{
				advance_level();
			}
			else if (ret == CI_MENU_TO_MAIN)
			{
				current_level = CI_LEVEL_MAINMENU;
			}
			else if (ret == CI_MENU_FIGHT)
			{
				current_level = next_fighting_level;
			}
			else if (ret == CI_STATE_LOST)
			{

			}
			else if (ret == CI_STATE_QUIT)
			{
				return 0;
			}
			else if (ret == CI_LEVEL_MAINMENU || ret == CI_LEVEL_SPEEDMENU || ret == CI_LEVEL_UPMENU)
			{
				current_level = ret;
			}
			
		}
	}


	kenira::console::set_console_cursor_visibility(true);
	system("CLS");
	return 0;
}

void Console_Invaders::initialize_game()
{
	kenira::console::resize_console_window(win_size[0], win_size[1]);
	time_start = std::chrono::high_resolution_clock::now();
	time_last_escape = time_start;

	current_level = CI_LEVEL_MAINMENU;
	next_fighting_level = CI_LEVEL_01;
	//next_fighting_level = CI_LEVEL_GAMEOVER;		// to test levels

	dim.x = win_size[0] - 1;
	dim.y = win_size[1] - 1;
	SetConsoleTitle("CONSOLE INVADERS YARRRRRRRRR");
	kenira::console::set_console_cursor_visibility(false);

	system("CLS");
	state = CI_STATE_ND;
	//init(win_size);

	upgrades.shot_damage = 1;
}

void Console_Invaders::advance_level()
{
	if (current_level == CI_LEVEL_SPEEDMENU)
	{
		current_level = CI_LEVEL_MAINMENU;
	}
	else if (current_level == CI_LEVEL_UPMENU)
	{
		current_level = CI_LEVEL_MAINMENU;
	}
	else if (current_level == CI_LEVEL_01)
	{
		next_fighting_level = CI_LEVEL_02;
		current_level = CI_LEVEL_MAINMENU;
	}
	else if (current_level == CI_LEVEL_02)
	{
		next_fighting_level = CI_LEVEL_03;
		current_level = CI_LEVEL_MAINMENU;
	}
	else if (current_level == CI_LEVEL_03)
	{
		next_fighting_level = CI_LEVEL_04;
		current_level = CI_LEVEL_MAINMENU;
	}
	else if (current_level == CI_LEVEL_04)
	{
		next_fighting_level = CI_LEVEL_05;
		current_level = CI_LEVEL_MAINMENU;
	}
	else if (current_level == CI_LEVEL_05)
	{
		next_fighting_level = CI_LEVEL_01;
		current_level = CI_LEVEL_GAMEWON;
	}

	if (current_level == CI_LEVEL_UPMENU && upgrade_points <= 0)
	{
		advance_level();
	}
}
void Console_Invaders::update_state()
{
	if (state != CI_STATE_QUIT && lives <= 0)
	{
		state = CI_STATE_LOST;
		current_level = CI_LEVEL_GAMEOVER;
	}
}
void Console_Invaders::draw_text_centered(const types::vstr& vstr)
{
	kenira::console::write_console_output_block(vstr, (dim.x - kenira::io::get_max_string_length_in_vstr(vstr)) / 2, (dim.y - (int)vstr.size()) / 2);
}

int Level::run()
{
	initialize();

	while (level_state == CI_STATE_ND)
	{
		get_keys_and_wait();
		move();
		collision_detection();
		delete_dead_entities();
		check_if_menu_shot();
		shoot();
		++tick;
		draw();
		update_state();
	}
	return level_state;
}
void Level::initialize()
{
	//state = CI_STATE_ND;
	level_state = CI_STATE_ND;
	tick = 0;
	//time_interval_ms = 128;


	// Level speed_menu;
	// time_interval_ms = speed_menu.run();


	time_start = std::chrono::high_resolution_clock::now();
	time_last_speed_change = time_start;

	system("CLS");
}
int Level::get_player_index()
{
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].type == CI_TYPE_PLAYER)
		{
			return i;
		}
	}
	return -1;
}

bool Level::collision(const Entity& one, const Entity& two)
{
	int x1 = one.pos.x + one.box.pos_rel.x;
	int y1 = one.pos.y + one.box.pos_rel.y;
	int x2 = two.pos.x + two.box.pos_rel.x;
	int y2 = two.pos.y + two.box.pos_rel.y;

	if (one.box.collision_disabled || two.box.collision_disabled || x1 + one.box.dim.x < x2 || y1 + one.box.dim.y < y2
		|| x1 > x2 + two.box.dim.x || y1 > y2 + two.box.dim.y)
	{	
		return false;
	}
	return true;
}
void Level::collision_detection()
{
	for (int i = 0; i < entities.size(); ++i)
	{
		if (entities[i].type == CI_TYPE_PLAYER || entities[i].type >= CI_TYPE_ALIEN_BASIC)
		{
			for (int j = 0; j < entities.size(); ++j)
			{
				//if ( )//&& entities[j].type != CI_TYPE_PLAYER) //CI_TYPE_PROJECTILE
				if (j != i && collision(entities[i], entities[j]))
				{
					if (entities[i].health > 0 && entities[j].health > 0)
					{
						entities[j].health -= entities[i].collision_damage;
						entities[i].health -= entities[j].collision_damage;
						if (entities[j].type == CI_TYPE_PROJECTILE)
						{
							entities[j].health--;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < menu_entities.size(); ++i)
	{
		for (int j = 0; j < entities.size(); ++j)
		{
			if (collision(menu_entities[i], entities[j]))
			{
				if (menu_entities[i].health > 0 && entities[j].health > 0)
				{
					menu_entities[i].health -= entities[j].collision_damage;
					entities[j].health--;
				}
			}
		}
	}
}
void Level::delete_dead_entities()
{
	for (int i = (int)entities.size() - 1; i >= 0; --i)
	{
		if (entities[i].health <= 0 || (entities[i].life_span_ms >= 0 && io::time_since_ms(entities[i].time_created) >entities[i].life_span_ms))
		{
			entities.erase(entities.begin() + i);
		}
	}

	for (int i = (int)menu_entities.size() - 1; i >= 0; --i)
	{
		if (menu_entities[i].health <= 0)
		{
			if (menu_entities[i].type != CI_TYPE_TEXT)
			{
				indices_menu_entities_shot.insert(i);
				menu_entities[i].health = menu_entities[i].max_health;
			}
			else if (menu_entities[i].life_span_ms >= 0 && io::time_since_ms(menu_entities[i].time_created) > menu_entities[i].life_span_ms)
			{
				menu_entities.erase(menu_entities.begin() + i);
			}
		}
	}
}
void Level::check_if_menu_shot()
{
	for (auto&& index : indices_menu_entities_shot)
	{
		if (menu_entities[index].type == CI_MENU_TO_MAIN)
		{
			r_current_level = CI_LEVEL_MAINMENU;
			level_state = r_current_level;
		}
		else if (menu_entities[index].type == CI_MENU_RESTART_GAME)
		{
			r_current_level = CI_LEVEL_MAINMENU;
			level_state = r_current_level;
			r_upgrades.reset();
			r_upgrade_points = 0;
			r_lives = 3;	// TODO lives_start
		}
		else if (menu_entities[index].type_parent == CI_LEVEL_MAINMENU)
		{
			if (menu_entities[index].effect_int == CI_STATE_QUIT)
			{
				level_state = CI_STATE_QUIT;
			}
			else
			{
				//r_current_level = menu_entities.back().effect_int;
				//level_state = CI_STATE_COMPLETE;
				level_state = menu_entities[index].effect_int;
			}
		}
		else if (menu_entities[index].type_parent == CI_LEVEL_SPEEDMENU)
		{
			r_time_interval_ms = menu_entities[index].time_interval;
			//level_state = CI_STATE_COMPLETE;
			//menu_entities[index].effect_int;
		}
		else if(menu_entities[index].type_parent == CI_LEVEL_UPMENU)
		{
			if (r_upgrade_points >= menu_entities[index].cost)
			{
				Entity temp({ 0,0 }, { 0,0 }, CI_TYPE_PLAYER, CI_TYPE_ND);

				bool upgrade_valid = true;

				if (menu_entities[index].effect.shot_delay != 0)
				{
					if (r_upgrades.shot_delay / menu_entities[index].effect.shot_delay >= UPGRADE_SHOOTING_DELAY_MAX)
					{
						upgrade_valid = false;
						draw_text("YOU'RE SHOOTING FAST ENOUGH LADSIE!", { 23, 15 }, 2000);
					}
				}
				else if (menu_entities[index].effect.precharge_delay != 0)
				{
					if (r_upgrades.precharge_delay / menu_entities[index].effect.precharge_delay >= UPGRADE_PRECHARGE_DELAY_MAX)
					{
						upgrade_valid = false;
						draw_text("YOU'RE CHARGING FAST ENOUGH LADSIE!", { 23, 15 }, 2000);
					}
				}

				if (upgrade_valid)
				{
					r_upgrades += menu_entities[index].effect;
					r_upgrade_points -= menu_entities[index].cost;
				}
			}
			else
			{
				draw_text("NOT ENOUGH UPGRADE POINTS LADSIE!", { 23, 17 }, 2000);
			}
			// TODO: if quit, then state complete
		}



		
	}
	indices_menu_entities_shot.clear();
}
void Level::move()
{
	bool alien_touching_vertical_border = false;
	bool alien_touching_horizontal_border = false;

	for (auto&& ent : entities)
	{
		if (ent.health > 0)
		{
			if (ent.pos.x + ent.vel.x <= border_left || ent.pos.x + ent.dimensions.x + ent.vel.x >= dim.x - border_right + 1)
			{
				if (ent.type >= CI_TYPE_ALIEN_BASIC)
				{
					alien_touching_horizontal_border = true;
				}
				else if (ent.type == CI_TYPE_PLAYER)
				{
					ent.vel.x = 0;
				}
				else if (ent.type == CI_TYPE_PROJECTILE)
				{
					ent.health--;
				}
			}
			if (ent.pos.y + ent.vel.y <= border_top || ent.pos.y + ent.dimensions.y + ent.vel.y >= dim.y - border_bottom + 1)
			{
				if (ent.type >= CI_TYPE_ALIEN_BASIC)
				{
					alien_touching_vertical_border = true;
				}
				else if (ent.type == CI_TYPE_PLAYER)
				{
					ent.vel.y = 0;
				}
				else if (ent.type == CI_TYPE_PROJECTILE)
				{
					ent.health--;
				}
				ent.vel.y = 0;
			}
		}
	}

	for (auto&& ent : entities)
	{
		if (alien_touching_horizontal_border && ent.type >= CI_TYPE_ALIEN_BASIC)
		{
			ent.vel.x *= -1;
		}
		if (alien_touching_vertical_border && ent.type >= CI_TYPE_ALIEN_BASIC)
		{
			ent.vel.y *= -1;
		}
		ent.pos += ent.vel;
	}
}
void Level::spawn_player(maths::Vec2i _pos, bool menu_mode)
{
	entities.push_back(Entity(_pos + maths::Vec2i(-2, -1), { 0,0 }, CI_TYPE_PLAYER, CI_TYPE_ND));
	if (menu_mode)
	{
		entities.back().shoot_delay = menu_shoot_delay;
	}
	else
	{
		entities.back().health += r_upgrades.health;
		entities.back().max_health += r_upgrades.health;
		entities.back().shoot_delay += r_upgrades.shot_delay;
		entities.back().precharge_delay += r_upgrades.precharge_delay;
		entities.back().shots_precharged_max += r_upgrades.precharge_max;
		entities.back().shot_damage_bonus += r_upgrades.shot_damage;
		entities.back().shot_damage_bonus_chance += r_upgrades.shot_damage_chance;
		entities.back().shot_duplicate_chance += r_upgrades.shot_duplicate_chance;
	}
}
void Level::spawn_projectile(int ind)
{
	Entity tempent(entities[ind].pos + entities[ind].projectile_spawn_offset, entities[ind].projectile_spawn_vel, CI_TYPE_PROJECTILE, entities[ind].type);
 	entities.push_back(tempent);	// ERROR
		
	int rand = kenira::maths::random_int(1, 100);
	int bonus = 0;
	if (entities[ind].shot_damage_bonus_chance >= 0 && rand <= entities[ind].shot_damage_bonus_chance)
	{
		entities.back().collision_damage += entities[ind].shot_damage_bonus;
		bonus = entities[ind].shot_damage_bonus;
	}

	rand = kenira::maths::random_int(1, 100);
	int temp = entities[ind].shot_duplicate_chance;

	if (entities[ind].shot_duplicate_chance >= 0 && rand <= entities[ind].shot_duplicate_chance)
	{
		maths::Vec2i offset2(1,0);

		tempent = Entity(entities[ind].pos + entities[ind].projectile_spawn_offset + offset2, entities[ind].projectile_spawn_vel, CI_TYPE_PROJECTILE, entities[ind].type);
		entities.push_back(tempent);	// closer spawn with -proj_spawn_vel
		entities.back().collision_damage += bonus;
	}
		
	entities[ind].tick_last_shot = tick;
	//std::cout << ent.tick_last_shot;
}
void Level::shoot()
{
	for (int i = 0; i < entities.size(); ++i)	//for (auto&& ent : entities)
	{
		if (((tick >= entities[i].tick_last_shot + std::max(1, entities[i].shoot_delay)) || entities[i].shots_precharged > 0) && (entities[i].shoot_delay >= 0) && (entities[i].health > 0))
		{
			if (entities[i].type >= CI_TYPE_ALIEN_BASIC)
			{
				spawn_projectile(i);
			}
			else if (entities[i].type == CI_TYPE_PLAYER)
			{
				if (entities[i].shooting_queued)
				{
					spawn_projectile(i);

					if (entities[i].shots_precharged > 0)
					{
						entities[i].shots_precharged--;
					}
					else
					{
						entities[i].shooting_queued = false;
					}
				}
				else if (!menu_mode)
				{
					int charges = (tick - entities[i].tick_last_shot - entities[i].shoot_delay) / (std::max(1, entities[i].shoot_delay + r_upgrades.precharge_delay));
					entities[i].shots_precharged = std::min(entities[i].shots_precharged_max, charges);
					
					if (entities[i].shots_precharged < 0)
					{
						entities[i].shots_precharged = 0;
					}
				}
			}
		}
		else if (menu_mode && entities[i].type == CI_TYPE_PLAYER)
		{
			entities[i].shooting_queued = false;
		}
	}
}
void Level::draw()
{
	system("CLS");

	for (auto&& ent : entities)
	{
		if (ent.health > 0)
		{
			kenira::console::write_console_output_block(ent.visual, ent.pos.x, ent.pos.y, ent.char_info_attribute);
		}
	}

	for (auto&& ent : menu_entities)
	{
		if (ent.health > 0)
		{
			kenira::console::write_console_output_block(ent.visual, ent.pos.x, ent.pos.y, ent.char_info_attribute);
		}
	}
	
	int player_index = get_player_index();
	if (player_index >= 0)
	{
		draw_stat(entities[player_index].health, "Health", "O", { dim.x - 16,0 }, 0x0c);
		draw_stat(entities[player_index].shots_precharged, "", "^", { entities[player_index].pos.x - entities[player_index].shots_precharged / 2 + 1, dim.y }, 0x0a);
	}
	draw_stat(r_lives, "Lives", "O", {2, 0}, 0x07);
	draw_stat(r_upgrade_points, "Upgrade Points", { dim.x/2-7, 0 }, 0x07);
}
void Level::draw_stat(int stat, std::string _stat_name, std::string unit_symbol, maths::Vec2i _pos, int attribute)
{
	std::string stat_name = _stat_name;
	if (_stat_name != "")
	{
		stat_name += " ";
	}
	for (int i = 0; i < stat; ++i)
	{
		stat_name += unit_symbol;
	}
	kenira::console::write_console_output_block({ stat_name }, _pos.x, _pos.y, attribute);
}
void Level::draw_stat(int stat, std::string _stat_name, maths::Vec2i _pos, int attribute)
{
	std::string stat_name = _stat_name + ": " + std::to_string(stat);
	kenira::console::write_console_output_block({ stat_name }, _pos.x, _pos.y, attribute);
}
void Level::draw_text(const std::vector<std::string>& vstr, maths::Vec2i _pos, int duration_ms)
{
	Entity ent(_pos, { 0,0 }, CI_TYPE_TEXT, CI_TYPE_ND);
	ent.visual = vstr;
	ent.life_span_ms = duration_ms;
	entities.push_back(ent);
	draw();
}
void Level::draw_text(const std::string& str, maths::Vec2i _pos, int duration_ms)
{
	draw_text(types::vstr{ str }, _pos, duration_ms);
}
void Level::draw_text(const types::vstr& vstr)
{
	kenira::console::write_console_output_block(vstr, 1, 1, 7);
}
void Level::draw_text(const std::string& str)
{
	draw_text(types::vstr{ str });
}
void Level::draw_text_centered(const types::vstr& vstr)
{
	kenira::console::write_console_output_block(vstr, (dim.x - kenira::io::get_max_string_length_in_vstr(vstr)) / 2 , (dim.y - (int)vstr.size()) / 2);
}
void Level::draw_text_centered(const std::string& str)
{
	draw_text_centered(types::vstr{ str });
}
bool Level::is_key_pressed(int key)
{
	return kenira::io::is_key_pressed_windows(key);
}
bool Level::change_speed(double factor)
{
	int* p_time_interval;
	if (menu_mode)
	{
		p_time_interval = &time_interval_ms_menu;
	}
	else
	{
		p_time_interval = &r_time_interval_ms;
	}

	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_last_speed_change).count() > speed_change_delay_ms)
	{
		*p_time_interval = (int)std::round(*p_time_interval / factor);		// division because factor 2 means double speed, so half the time interval
		*p_time_interval = std::min(*p_time_interval, time_interval_max);
		*p_time_interval = std::max(*p_time_interval, time_interval_min);
		time_last_speed_change = std::chrono::high_resolution_clock::now();
		return true;
	}
	return false;
}
void Level::get_keys_and_wait()
{												// OEM_1=Ü 2=# 3=Ö 4=ß 5=^ 6=´ 7=Ä 102=< 8 not found
	std::vector<int> keys_valid = {'A', 'D', VK_SPACE, VK_OEM_PLUS, VK_OEM_MINUS, VK_ADD, VK_SUBTRACT, VK_ESCAPE, 'O', 'P', VK_OEM_1, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7, VK_OEM_8, VK_OEM_102 };
	std::set<int> keys_pressed;
	//std::array<int, 2> keys_pressed{ 0,0 };							// first for direction (A or D), second spacebar (or not)

	auto time_since_last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_last_tick).count();	// in ms

	int interval_ms = r_time_interval_ms;
	if (menu_mode)
	{
		interval_ms = time_interval_ms_menu;
	}

	do
	{
		//int key = GetAsyncKeyState();
		//std::cout << "a";
		//if (GetAsyncKeyState('A') & 0x8000/*check if high-order bit is set (1 << 15)*/)
		//{
		//	keys_pressed[0] = 'A';				
		//}
		//else if (GetAsyncKeyState('D') & 0x8000/*check if high-order bit is set (1 << 15)*/)
		//{
		//	keys_pressed[0] = 'D';
		//}
		//
		//if (GetAsyncKeyState(VK_SPACE) & 0x8000/*check if high-order bit is set (1 << 15)*/)
		//{
		//	keys_pressed[1] = VK_SPACE;
		//}

		for (auto&& key : keys_valid)
		{
			if (is_key_pressed(key))
			{
				keys_pressed.insert(key);
			}
		}

		//int hello = std::chrono::milliseconds(std::min((long long)5, time_since_last_tick)).count();
		kenira::io::wait_ms((int)std::min((long long)5, time_since_last_tick));
		time_since_last_tick = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - time_last_tick).count();
	}
	while (time_since_last_tick < interval_ms);

	for (auto&& ent : entities)
	{
		if (ent.type == CI_TYPE_PLAYER)
		{
			ent.vel.x = 0;
			if (keys_pressed.find('A') != keys_pressed.end())
			{
				ent.vel.x -= 1;
			}
			if (keys_pressed.find('D') != keys_pressed.end())
			{
				ent.vel.x += 1;
			}
			if (keys_pressed.find(VK_OEM_PLUS) != keys_pressed.end() || keys_pressed.find(VK_ADD) != keys_pressed.end())
			{
				change_speed(2);
			}
			if (keys_pressed.find(VK_OEM_MINUS) != keys_pressed.end() || keys_pressed.find(VK_SUBTRACT) != keys_pressed.end())
			{
				change_speed(0.5);
			}
			if (keys_pressed.find('O') != keys_pressed.end())
			{
				level_state = CI_STATE_WON;
			}
			if (keys_pressed.find('P') != keys_pressed.end())
			{
				level_state = CI_STATE_LOST;
			}
			if (keys_pressed.find(VK_OEM_1) != keys_pressed.end())
			{
				r_upgrade_points += 100;
			}
			if (keys_pressed.find(VK_OEM_3) != keys_pressed.end()) 
			{
				--r_lives;
			}
			if (keys_pressed.find(VK_OEM_7) != keys_pressed.end())
			{
				++r_lives;
			}
			if (keys_pressed.find(VK_OEM_2) != keys_pressed.end())
			{
				
			}
			if (keys_pressed.find(VK_SPACE) != keys_pressed.end())
			{
				ent.shooting_queued = true;
			}
			if (keys_pressed.find(VK_ESCAPE) != keys_pressed.end() && io::time_since_ms(time_last_escape) > time_delay_escape_ms)
			{
				if (type > CI_LEVEL_MAINMENU && type <= CI_LEVEL_UPMENU)
				{
					level_state = CI_STATE_COMPLETE;
				}
				else if (type >= CI_LEVEL_01)
				{
					level_state = CI_LEVEL_MAINMENU;
				}
				else
				{
					level_state = CI_STATE_QUIT;
				}
				time_last_escape = std::chrono::high_resolution_clock::now();
			}
		}
	}

	time_last_tick = std::chrono::high_resolution_clock::now();
}
void Level::update_state()
{
	if (type >= CI_LEVEL_01)
	{
		if (level_state != CI_STATE_QUIT && std::find_if(entities.begin(), entities.end(), [](Entity ent) {return ent.type >= CI_TYPE_ALIEN_BASIC; }) == entities.end())
		{
			level_state = CI_STATE_WON;
		}
		if (level_state != CI_STATE_QUIT && std::find_if(entities.begin(), entities.end(), [](Entity ent) {return ent.type == CI_TYPE_PLAYER; }) == entities.end())
		{
			level_state = CI_STATE_LOST;
		}

		if (level_state == CI_STATE_WON)
		{
			int ind = get_player_index();
			int damage_taken = entities[ind].max_health - entities[ind].health;
			int points = std::max(0, 5 - damage_taken);
			r_upgrade_points += points;
			r_upgrade_points_by_level[r_current_level] = points;
			types::vstr vstr = { "A WINNER IS YOU! PREPARE THE PARTY CANNON!", "YOU EARNED " + std::to_string(points) + " UPGRADE POINTS FOR NOT TAKING DAMAGE! YAYS" };
			if (entities[get_player_index()].health == entities[get_player_index()].max_health)
			{
				r_lives++;
				vstr.push_back("YOU EVEN TOOK NO DAMAGE! EXTRA LIFE FOR YOU, PAL");
			}
			draw_text(vstr, { 12, 15 }, 2000);
			io::wait_ms(2500);
		}
		else if (level_state == CI_STATE_LOST)
		{
			r_lives--;
			draw_text("A LOSER IS YOU! SHAME...", { 20, 15 }, 2000);
			io::wait_ms(1000);
		}
	}
	else if (type == CI_LEVEL_MAINMENU)
	{

	}
	else if (type == CI_LEVEL_SPEEDMENU)
	{

	}
	else if (type == CI_LEVEL_UPMENU)
	{

	}
	
	
	
}


}
}