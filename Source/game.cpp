#include "game.h"
#include "gui.h"

#include "raymath.h"

#include "resources.h"
#include <iostream>
#include <stack>
#include <vector>

void Entity::update()
{
   

    if (wander && texture.id == res::get_or_load_texture("Assets/boss.png").id) //this is the wander for the boss so he stops every 0.5s
    {

        if (!is_moving) //this is only true once the boss is at the destination
        {
           
            wander_timer_seconds -= GetFrameTime(); //this is to have the pause 
            if (wander_timer_seconds <= 0) //once it hits zero the timer has run out and the boss can move again
            {
                is_moving = true;
                
                move_target.x = (float)GetRandomValue((int)radius, GetScreenWidth() - 2 * (int)radius); // these are for the random generation of an x and y coordinate on the screen for the enemies to move towards
                move_target.y = (float)GetRandomValue((int)radius, GetScreenHeight() - 2 * (int)radius);
                
            }
        }
        else
        {
            if (Vector2Distance(position, move_target) < 1)
            {
                wander_timer_seconds = (float)GetRandomValue(1, 5) / 10.0f; //this gets a random float between 0.1 seconds and 0.5 seconds for the timer
                is_moving = false;

            }
            
        }
    }
    else if (wander && texture.id == res::get_or_load_texture("Assets/minion.png").id) //to make sure that the minions are always moving and aren't effected by the bosses timer pause
    {
        if (Vector2Distance(position, move_target) < 1)
        {
            move_target.x = (float)GetRandomValue((int)radius, GetScreenWidth() - 2 * (int)radius); // these are for the random generation of an x and y coordinate on the screen for the enemies to move towards
            move_target.y = (float)GetRandomValue((int)radius, GetScreenHeight() - 2 * (int)radius);
        }
    }
    


    if(die_on_target_reached)
    {
        if(Vector2Distance(position, move_target) < 1)
        {
            health = 0;
            PlaySound(res::get_or_load_sound("Assets/hit_ground.wav"));
        }
    }

    if(move_to_target)
    {
        position = Vector2MoveTowards(position, move_target, move_speed * GetFrameTime()/2);
    }
}

 
void Entity::draw() const
{
    if(texture.id)
    {
        Vector2 texture_top_left = position;
        texture_top_left.x -= texture.width  / 2;
        texture_top_left.y -= texture.height / 2;

        DrawTextureV(texture, texture_top_left, WHITE);
    }
    else
    {
        DrawCircleV(position, radius, color);
    }
}

void Entity::on_collision(Entity& other) //For collisions it was becoming difficult to differentiate between enemies and bullets so i decided to manually set what happens to a player, enemy or bullet in each scenario depending on whats colliding 
{
    if (deal_damage_on_collision) {
        Entity* player = Game::get_entity(Game::player_id);
        if (player && this == player && other.texture.id == res::get_or_load_texture("Assets/minion.png").id)
        {
            other.health = 0; //so minion "explodes"
            this->take_damage(other, Game::PROJECTILE_DAMAGE);
        }
        else if (player && this == player && other.texture.id == res::get_or_load_texture("Assets/bomb.png").id)
        {
            this->health -= 5;
            this->isdead = true;
            Game::game_is_dead = true;
            Game::push_game_state(GameState::LOSE);
            return;
        }
        else if (this->texture.id == res::get_or_load_texture("Assets/minion.png").id && other.texture.id == res::get_or_load_texture("Assets/bomb.png").id)
        {
            return;
        }
        else if (this->texture.id == res::get_or_load_texture("Assets/bullet.png").id && other.texture.id == res::get_or_load_texture("Assets/bomb.png").id)
        {
            this->health = 0;
            other.take_damage(*this, Game::PROJECTILE_DAMAGE);
        }
        else if (this->texture.id == res::get_or_load_texture("Assets/bullet.png").id && other.texture.id == res::get_or_load_texture("Assets/minion.png").id)
        {
            this->health = 0;
            other.health -= 1;
        }
        else if (this->texture.id == res::get_or_load_texture("Assets/bullet.png").id && other.texture.id == res::get_or_load_texture("Assets/boss.png").id)
        {
            this->health = 0;
            other.take_damage(*this, Game::PROJECTILE_DAMAGE);
        }
       
    }
}



// [[maybe_unused]] tells the compiler not to warn us if we don't use the parameter

void Entity::take_damage([[maybe_unused]] Entity& from, float damage)
{
    // If we want to deal damage back here, based on our behavior, we could call from->take_damage(this, ...)

    health -= damage;

    PlaySound(res::get_or_load_sound("Assets/hit.wav"));

    if (health <= 0) // similar to the on collision
    {
        health = 0;
        isdead = true;
        PlaySound(res::get_or_load_sound("Assets/hit_ground.wav"));
        Entity* player = Game::get_entity(Game::player_id);
        if (player && this == player) {
            Game::game_is_dead = true;
            Game::push_game_state(GameState::LOSE);

        }
    }

}

namespace Game
{
    std::stack<GameState> state_stack;

    std::vector<Entity> entities = {};

    bool game_is_dead = false;
    size_t player_id = 0;

    // We moved to using a vector of entities, a vector can resize when we add entities to it,
    // which means that pointers to data in the vector might become invalid.
    // Therefore we use indices instead.
    // ID 0 is invalid! See the functions get_entity() and add_entity(), those are the ones that the game uses to create and get entities.
    // No other piece of code should be touching the "entities" vector.

    bool running;
    void push_game_state(GameState state)
    {
        state_stack.push(state); //to store the different states of the game like the menu, win and lose screen
    }
	
    void pop_game_state()
    {
        state_stack.pop();
    }

    Entity* get_entity(size_t id)
    {
        if(id == 0 || id > entities.size()) return nullptr;

        return &entities[id - 1];
    }

    size_t add_entity(const Entity& entity)
    {
        entities.emplace_back(entity);
        return entities.size();
    }

	void init()
	{
        game_is_dead = false; // initially set the game_is_dead to false so that when the player dies, this bool becomes true and triggers the lose condition
        running = true;
        push_game_state(GameState::MAIN_MENU);
        
        // Preload all assets
        res::get_or_load_sound("Assets/shoot.wav");
        res::get_or_load_sound("Assets/hit_ground.wav");
        res::get_or_load_sound("Assets/hit.wav");

        res::get_or_load_texture("Assets/player.png");
        res::get_or_load_texture("Assets/bullet.png");
        res::get_or_load_texture("Assets/enemy.png");
        res::get_or_load_texture("Assets/boss.png");
        res::get_or_load_texture("Assets/minion.png");
        res::get_or_load_texture("Assets/bomb.png");
        
        Entity player;
        player.move_to_target           = false;
        player.deal_damage_on_collision = true;
        player.color    = PLAYER_COLOR;
        player.health   = 5;
        player.isdead = false;
        player.radius   = 8;
        player.texture  = res::get_or_load_texture("Assets/player.png");
        player_id = add_entity(player);
               

        for(size_t i = 0; i < 10; i++)
        {
            Entity minion_entity = {};
            minion_entity.move_to_target = true;
            minion_entity.wander         = true;
            minion_entity.deal_damage_on_collision = true;
           

            minion_entity.radius = 8; // px

            minion_entity.position.x = (float) GetRandomValue((int)minion_entity.radius, GetScreenWidth()  - 2 * (int)minion_entity.radius);
            minion_entity.position.y = (float) GetRandomValue((int)minion_entity.radius, GetScreenHeight() - 2 * (int)minion_entity.radius);

            minion_entity.move_target = minion_entity.position;
            minion_entity.color       = GREEN;
            minion_entity.health      = 3; // I know this is strange as it should be 2 but with the collision of the bullet and minion it would die after colliding once with a bullet so this was a way to fix it
            minion_entity.move_speed  = Entity::WANDER_SPEED;
            minion_entity.texture     = res::get_or_load_texture("Assets/minion.png");


            add_entity(minion_entity);
        }

        
        for (size_t i = 0; i < 5; i++)
        {
            Entity bomb_entity = {};
            bomb_entity.move_to_target = false;
            bomb_entity.wander = false;
            bomb_entity.deal_damage_on_collision = true; // by setting this true only another entity which also has this set as true can collide and be dealt damage by the bomb

            bomb_entity.radius = 4; // px

            bomb_entity.position.x = (float)GetRandomValue((int)bomb_entity.radius, GetScreenWidth() - 2 * (int)bomb_entity.radius);
            bomb_entity.position.y = (float)GetRandomValue((int)bomb_entity.radius, GetScreenHeight() - 2 * (int)bomb_entity.radius);

            bomb_entity.color = BLUE;
            bomb_entity.health = 5;
            bomb_entity.texture = res::get_or_load_texture("Assets/bomb.png");


            add_entity(bomb_entity);
        }

        Entity boss_entity = {};
        boss_entity.move_to_target = true;
        boss_entity.wander = true;
        boss_entity.deal_damage_on_collision = false;

        boss_entity.radius = 4; // px

        boss_entity.position.x = (float)GetRandomValue((int)boss_entity.radius, GetScreenWidth() - 2 * (int)boss_entity.radius);
        boss_entity.position.y = (float)GetRandomValue((int)boss_entity.radius, GetScreenHeight() - 2 * (int)boss_entity.radius);

        boss_entity.move_target = boss_entity.position; 
        boss_entity.color = GREEN;
        boss_entity.health = 40;
        boss_entity.move_speed = Entity::WANDER_SPEED / 2;
        boss_entity.texture = res::get_or_load_texture("Assets/boss.png");


        add_entity(boss_entity);
	}
    float get_boss_x()
    {
        for (const auto& entity : entities) // searches through all entities
        {
            if (entity.texture.id == res::get_or_load_texture("Assets/boss.png").id) //makes sure that the entity we want position from is the boss by comparing its texture
            {
                return float(entity.position.x); //returns the x value of the bosses position in game
            } 
            
            
        }
        return 0; //if the boss isn't alive
        
    }
    float get_boss_y()
    {
        
        for (const auto& entity : entities)
        {
            if (entity.texture.id == res::get_or_load_texture("Assets/boss.png").id)
            {
                return float(entity.position.y);
            }
           
            
        }
        return 0;
       
    }

    static float spawn_timer = 0.5f;
    static constexpr float spawn_wait = 0.5f;
    void spawner() //I made a function which spawns a minion every 5 seconds next to the boss 
    {
        bool boss_alive = false;
        for (const auto& entity : entities) // this is important because without it the minions would continue spawning after boss died 
        {
            if (!entity.isdead && entity.texture.id == res::get_or_load_texture("Assets/boss.png").id)
            {
                boss_alive = true; //this means we can now spawn minions
                break;
            }
        }
        if (boss_alive)
        {
            spawn_timer -= GetFrameTime();
            if (spawn_timer <= 0)
            {
                Entity minion_entity = {};
                minion_entity.move_to_target = true;
                minion_entity.wander = true;
                minion_entity.deal_damage_on_collision = true;


                minion_entity.radius = 8; // px

                minion_entity.position.x = get_boss_x(); //this is pulled from the function
                minion_entity.position.y = get_boss_y(); //this is also pulled from the function

                minion_entity.move_target = minion_entity.position;
                minion_entity.color = GREEN;
                minion_entity.health = 3; // I know this is strange as it should be 2 but with the collision of the bullet and minion it would die after colliding once with a bullet so this was a way to fix it
                minion_entity.move_speed = Entity::WANDER_SPEED;
                minion_entity.texture = res::get_or_load_texture("Assets/minion.png");

                add_entity(minion_entity);
                spawn_timer += spawn_wait;
            }
        }
    }
    bool should_quit()
    {
        return !running || WindowShouldClose();
    }

	void deinit()
	{
        res::unload();
	}

    static void shoot_projectile(Vector2 position, Vector2 target)
    {
        Entity projectile = {};
        projectile.move_to_target           = true;
        projectile.deal_damage_on_collision = true;
        projectile.die_on_target_reached    = true;
        projectile.position    = position;
        projectile.move_target = target;
        projectile.color       = GRAY;
        projectile.health      = 1; //this makes sure that the projectiles aren´t causing too much damage
        projectile.radius      = 6; // px
        projectile.move_speed  = Entity::PROJECTILE_SPEED;
        projectile.texture     = res::get_or_load_texture("Assets/bullet.png");

        add_entity(projectile);

        PlaySound(res::get_or_load_sound("Assets/shoot.wav"));
    }
    
    static void do_main_menu() // the main menu for the game linked with gui
    {
        ClearBackground(MENU_BACKGROUND_COLOR);

        constexpr int menu_item_count = 2;
        int current_item = 0;

        float menu_total_height = menu_item_count * MENU_BUTTON_HEIGHT + (menu_item_count - 1) * MENU_BUTTON_MARGIN;
        float menu_start_y      = (GetScreenHeight() - menu_total_height) / 2.f;

        {
            Rectangle start_button = {};
            start_button.width  = MENU_BUTTON_WIDTH;
            start_button.height = MENU_BUTTON_HEIGHT;
            start_button.x      = (GetScreenWidth()  - MENU_BUTTON_WIDTH)  / 2.f;
            start_button.y      = menu_start_y + current_item * (MENU_BUTTON_HEIGHT + MENU_BUTTON_MARGIN);

            const char* start_button_text = "START";

            if(gui_button(start_button, start_button_text))
            {
                entities.clear(); //did this at the start of menu so when you die or win the game restarts again
                player_id = 0;
                game_is_dead = false;
                init();
                push_game_state(GameState::GAME);
            }

            current_item += 1;
        }

        {
            Rectangle quit_button = {};
            quit_button.width  = MENU_BUTTON_WIDTH;
            quit_button.height = MENU_BUTTON_HEIGHT;
            quit_button.x      = (GetScreenWidth()  - MENU_BUTTON_WIDTH)  / 2.f;
            quit_button.y      = menu_start_y + current_item * (MENU_BUTTON_HEIGHT + MENU_BUTTON_MARGIN);

            const char* quit_button_text = "QUIT";

            if(gui_button(quit_button, quit_button_text))
            {
                running = false;
            }

            current_item += 1;
        }
    }
    static void do_lose_menu() // repeated the same as main menu but changed the background colour to red and added a main menu button and quit button
    {
        ClearBackground(LOSE_BACKGROUND_COLOR);

        constexpr int menu_item_count = 2;
        int current_item = 0;

        float menu_total_height = menu_item_count * MENU_BUTTON_HEIGHT + (menu_item_count - 1) * MENU_BUTTON_MARGIN;
        float menu_start_y = (GetScreenHeight() - menu_total_height) / 2.f;

        {
            Rectangle start_button = {};
            start_button.width = MENU_BUTTON_WIDTH;
            start_button.height = MENU_BUTTON_HEIGHT;
            start_button.x = (GetScreenWidth() - MENU_BUTTON_WIDTH) / 2.f;
            start_button.y = menu_start_y + current_item * (MENU_BUTTON_HEIGHT + MENU_BUTTON_MARGIN);

            const char* start_button_text = "MAIN MENU";

            if (gui_button(start_button, start_button_text))
            {
                push_game_state(GameState::MAIN_MENU);
            }

            current_item += 1;
        }

        {
            Rectangle quit_button = {};
            quit_button.width = MENU_BUTTON_WIDTH;
            quit_button.height = MENU_BUTTON_HEIGHT;
            quit_button.x = (GetScreenWidth() - MENU_BUTTON_WIDTH) / 2.f;
            quit_button.y = menu_start_y + current_item * (MENU_BUTTON_HEIGHT + MENU_BUTTON_MARGIN);

            const char* quit_button_text = "QUIT";

            if (gui_button(quit_button, quit_button_text))
            {
                running = false;
            }

            current_item += 1;
        }
    }

    static void do_win_menu() // same once again except this has a green background
    {
        ClearBackground(WIN_BACKGROUND_COLOR);

        constexpr int menu_item_count = 2;
        int current_item = 0;

        float menu_total_height = menu_item_count * MENU_BUTTON_HEIGHT + (menu_item_count - 1) * MENU_BUTTON_MARGIN;
        float menu_start_y = (GetScreenHeight() - menu_total_height) / 2.f;

        {
            Rectangle start_button = {};
            start_button.width = MENU_BUTTON_WIDTH;
            start_button.height = MENU_BUTTON_HEIGHT;
            start_button.x = (GetScreenWidth() - MENU_BUTTON_WIDTH) / 2.f;
            start_button.y = menu_start_y + current_item * (MENU_BUTTON_HEIGHT + MENU_BUTTON_MARGIN);

            const char* start_button_text = "MAIN MENU";

            if (gui_button(start_button, start_button_text))
            {
                push_game_state(GameState::MAIN_MENU);
            }

            current_item += 1;
        }

        {
            Rectangle quit_button = {};
            quit_button.width = MENU_BUTTON_WIDTH;
            quit_button.height = MENU_BUTTON_HEIGHT;
            quit_button.x = (GetScreenWidth() - MENU_BUTTON_WIDTH) / 2.f;
            quit_button.y = menu_start_y + current_item * (MENU_BUTTON_HEIGHT + MENU_BUTTON_MARGIN);

            const char* quit_button_text = "QUIT";

            if (gui_button(quit_button, quit_button_text))
            {
                running = false;
            }

            current_item += 1;
        }
    }
    

    static void control_entity(Entity& possessed)
    {
        if (possessed.isdead) { //this is supposed to check whether the main player has died by seeing if the isdead bool was turned to true when their health went to 0 or below
            return;
        }
        Vector2 direction = {};

        if(IsKeyDown(KEY_A)) // movement
        {
            direction.x += -1; 
            
        }

        if(IsKeyDown(KEY_D))
        {
            direction.x += 1; 
            
        }

        if(IsKeyDown(KEY_W))
        {
            direction.y += -1; 
            
        }

        if(IsKeyDown(KEY_S))
        {
            direction.y += 1;
            
        }


        direction = Vector2Normalize(direction);

        Vector2 position_next_frame = Vector2Add(possessed.position, Vector2Scale(direction, PLAYER_SPEED * GetFrameTime()));

        if (position_next_frame.x > GetScreenWidth() - possessed.radius) { //this part is to make sure that the player stays within the bounds of the game
            position_next_frame.x = GetScreenWidth() - possessed.radius;
        }
        if (position_next_frame.y > GetScreenHeight() - possessed.radius) {
            position_next_frame.y = GetScreenHeight() - possessed.radius;
        }
        if (position_next_frame.x < possessed.radius) {
            position_next_frame.x = possessed.radius;
        }
        if (position_next_frame.y < possessed.radius) {
            position_next_frame.y = possessed.radius;
        }

        possessed.position = position_next_frame;
       
        Vector2 shoot_direction = {};
        if(IsKeyPressed(KEY_LEFT)) // this is to control the bullets and shooting mechanics based on the key that is pressed
        {
            shoot_direction.x += -1; 
        }

        if(IsKeyPressed(KEY_RIGHT))
        {
            shoot_direction.x += 1; 
        }

        if(IsKeyPressed(KEY_UP))
        {
            shoot_direction.y += -1; // This one looks odd but remember that most of the times the Y axis goes from the top to the bottom.
        }

        if(IsKeyPressed(KEY_DOWN))
        {
            shoot_direction.y += 1;
        }

        shoot_direction = Vector2Normalize(shoot_direction);
                    
        if(Vector2LengthSqr(shoot_direction) > 0)
        {
            Vector2 shoot_position  = Vector2Add(possessed.position, Vector2Scale(shoot_direction, 2 * possessed.radius));
            Vector2 target_position = Vector2Add(possessed.position, Vector2Scale(shoot_direction, SHOOT_DISTANCE));

            shoot_projectile(shoot_position, target_position);
        }
    }

    static void do_game()
    {
        spawner(); //so minions can spawn
        
        if (state_stack.top() == GameState::LOSE) //to make sure lose screen works as I had issues with it before
        {
            return;
        }

        if(IsKeyPressed(KEY_ESCAPE))
        {
            pop_game_state();
            return;
        }
       
        if (entities.size() <= 1 && player_id == 1) { //this is for the win screen I figured by finding if there is only one entity left and the player is still alive because its id i 1 then it must be a win
            push_game_state(GameState::WIN);
            return;
        }
    
        if (player_id != 0) { //so if player is alive
            Entity* player = get_entity(player_id);
            if (player && player->isdead) { //this was also added because the player wouldn't die for weeks so this double checks the player dies
                game_is_dead = true;
                push_game_state(GameState::LOSE);
                return;
            }
        }
      
        if(player_id != 0){
            Entity* possessed = get_entity(player_id);
            if (possessed && !possessed->isdead) {
                control_entity(*possessed); // to make sure only the player is controllable which was also an issue I had for a while
            }
           
        }

        
                 
        // Update all entities
        for(Entity& entity : entities)
        {
            entity.update();//constantly updating the entities in the game

        }

        // Detect collisions
        for(Entity& a : entities) 
        {
            for(Entity& b : entities)
            {
                if(&a == &b) continue; // An entity should not collide with itself, skip it

                if(CheckCollisionCircles(a.position, a.radius, b.position, b.radius))
                {
                    a.on_collision(b);
                    b.on_collision(a);
                }
            }
        }

        // Remove all entities with negative or zero health.
        const auto last_entity_alive = std::remove_if(begin(entities), end(entities), 
            [](const Entity& entity) -> bool
            {
                return entity.health <= 0 && entity.player_id != player_id; //this removes the entity
            }
        );

        entities.erase(last_entity_alive, end(entities));

        
        // Render Frame

        ClearBackground(BLUE);

        Rectangle controls_rect = {};
        controls_rect.x = INITIAL_WINDOW_WIDTH / 2;
        controls_rect.y = INITIAL_WINDOW_HEIGHT / 2;
        controls_rect.width  = 0;
        controls_rect.height = 0;


        gui_text(controls_rect, "ESC: Main Menu, WASD: Move, Arrows: Shoot", CONTROLS_TEXT_COLOR);

        // Draw all entities
        for(const Entity& entity : entities)
        {
            entity.draw();
        }
        
        
    }

    void draw_frame()
	{

        GameState current_game_state = state_stack.top();

        switch(current_game_state)
        {
        case GameState::MAIN_MENU:
            do_main_menu();
            break;
        case GameState::GAME:
            do_game();
            break;
        case GameState::LOSE:
            do_lose_menu();
            break;
        case GameState::WIN:
            do_win_menu();
            break;

        }
	}
}