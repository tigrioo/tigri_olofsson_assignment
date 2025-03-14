#pragma once

#include "gui.h"
#include "raylib.h"

enum struct GameState
{
	MAIN_MENU,
	GAME,
	WIN,
	LOSE
};

struct Entity
{
	static constexpr float WANDER_SPEED     = 200.f; // pixels/s
	static constexpr float PROJECTILE_SPEED = 800.f; // pixels/s

	/*
	* Instead of using a class hierarchy, I decided to only have one kind of entity and enable behaviors with boolean variables.
	* The advantage of this is that we only have one Entity type, and therefore can store that easily in a container like vector, 
	* and we also have maximum flexibility in terms of what our entities can do. For example, if I want a bullet to not die when
	* it collides with something I can just set "deal_damage_on_collision" to false. Or, let's say I want to have projectiles that
	* randomly move around the map, you can set "wander" to true and they will start doing it.
	* This is not necessarily the best approach in all cases, the more data you add to the entity the bigger it will get and if you
	* have an entity that needs _a lot_ while most others don't need 90% of the members. You might waste a considerable amount of space.
	*/

	bool wander;
	bool move_to_target;
	bool deal_damage_on_collision;
	bool die_on_target_reached;
	bool isdead = false;
	Texture2D texture;

	Color color;
    float radius;

    Vector2 position;
    Vector2 move_target;
    float   move_speed;

	float health;

	void update();
	void draw() const;

	void on_collision(Entity& other); // The game code calls this function every frame where the two entities are colliding.
	void take_damage(Entity& from, float damage);
	size_t possessed_entity_id = 0;
};

namespace Game 
{	
	constexpr int INITIAL_WINDOW_WIDTH  = 800;
	constexpr int INITIAL_WINDOW_HEIGHT = 450;

	constexpr Color PLAYER_COLOR = Color{80, 80, 160, 255};
	constexpr Color ENEMY_COLOR  = Color{160, 80, 80, 255};

	constexpr Color CONTROLS_TEXT_COLOR = Color{30, 30, 60, 255};

	constexpr Color MENU_BACKGROUND_COLOR = Color{40, 40, 40, 255};
	constexpr Color GAME_BACKGROUND_COLOR = Color{40, 40, 80, 255};
	constexpr Color LOSE_BACKGROUND_COLOR = Color{ 255, 0, 0, 150 };
	constexpr Color WIN_BACKGROUND_COLOR = Color{ 0, 255, 0, 150 };

	constexpr int MENU_BUTTON_WIDTH  = 150;
	constexpr int MENU_BUTTON_HEIGHT = FONT_SIZE + 20;
	constexpr int MENU_BUTTON_MARGIN = FONT_SIZE / 2;

	constexpr float PLAYER_SPEED      = 400.f; // pixels/s
	constexpr float PLAYER_MAX_HEALTH = 5;
	constexpr float ENEMY_MAX_HEALTH  = 2;

	constexpr float PROJECTILE_DAMAGE = 1;
	constexpr float SHOOT_DISTANCE    = 200.f;

	void push_game_state(GameState state);
	void pop_game_state();

	Entity* get_entity(size_t id);
	size_t add_entity(const Entity& entity);

	void init();
	void deinit();

	bool should_quit();
	void draw_frame();
	extern bool game_is_dead;
	
};