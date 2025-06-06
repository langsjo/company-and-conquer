#include "renderer.hpp"
#include "rendering_engine.hpp"
#include "shop_ui.hpp"
#include "tinyfiledialogs.h"
#include "main_screen.hpp"


/**
 * @brief Construct a new Renderer::Renderer object that will handle the logic for rendering different parts
 * 
 * @param width the width of the window
 * @param height the height of the window
 */
Renderer::Renderer( size_t width, size_t height ) : width_(width), height_(height)
{
    map_text_path_ = TEXTURE_PATH;
    unit_text_path_ = UNITS_TEXTURE_PATH;
    building_text_path_ = BUILDINGS_TEXTURE_PATH;
    aux_text_path_ = AUX_TEXTURE_PATH;
    text_font_path_ = FONT_PATH;

    // we initialise the objects for the game, window, and the renderables
    game_ = std::make_shared<Game>( height, width );
    
    //Disallow resizing
    render_window_ = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), "Game", sf::Style::Close | sf::Style::Titlebar);
    renderables_ = std::make_shared<Window_To_Render>();

}


void Renderer::initialise_level( size_t level_idx )
{
    // used for storing the tiles corresponding character in the level map
    std::vector<std::vector<char>> terrain_vec;

    if ( level_idx == 0 ) {
        terrain_vec = builder_.read_map_file(TESTMAP_PATH);
    }
    if ( level_idx == 1 ) {
        terrain_vec = builder_.read_map_file(TESTMAP_PATH1);
    }


    size_t test_map_height = terrain_vec.size();
    size_t test_map_width = terrain_vec[0].size();

    // create the new Game object with the new level
    game_ = std::make_shared<Game>( test_map_width, test_map_height );
    logs_ = std::make_shared<Game_Logs>(10);


    // store the pointer to the new level into the <tile_map_>, and
    // add it into the renderable,
    // also do this for every other object
    tile_map_ = std::make_shared<Tile_Map>( game_, 100 );
    r_map_ = std::make_shared<Render_Map>( tile_map_ );
    r_units_ = std::make_shared<Render_Units>( tile_map_ );
    r_buildings_ = std::make_shared<Render_Buildings>( tile_map_ );
    r_aux_ = std::make_shared<Render_Aux>( tile_map_ ); 
    

    // clear out the old renderables
    renderables_->clear();

    // add all the renderables into one object
    // that will be called to make rendering more straightforward
    renderables_->add_drawable( r_map_ );
    renderables_->add_drawable( r_buildings_ );
    renderables_->add_drawable( r_units_ );
    renderables_->add_drawable( r_aux_ );

    
    if (!r_map_->load(map_text_path_)) {
        return;
    }
    if (!r_units_->load(unit_text_path_)) {
        return;
    }

    if (!r_buildings_->load(building_text_path_)) {
        return;
    }

    if (!r_aux_->load(aux_text_path_, text_font_path_)) {
        return;
    }
    
    game_->init_game();

    window_.get_game() = game_;
}

bool Renderer::load_scenario()
{
    // Open system file selector
    char const * filters[1] = { "*.yaml" };
    char const * selection = tinyfd_openFileDialog("Select a scenario", SCENARIOS_PATH, 1, filters, NULL, 0);

    if (selection == nullptr){
        std::cout << "No scenario was selected, returning to main menu" << std::endl;
        return false;
    }

    try {
        ScenarioLoader loader = ScenarioLoader(selection);
        scenario_ = std::make_shared<Scenario>(loader.load_scenario());
    } catch (std::runtime_error error) {
        std::cout << error.what() << "\n" << "Returning to main menu" << std::endl;
        return false;
    }

    return true;
}

/**
 * Create and initialize the shop UI from currently loaded scenario
 */
void Renderer::start_shop()
{
    Shop& shop = scenario_->get_shop();
    shop_ui_ = std::make_shared<ShopUI>(shop, *this);
    shop_ui_->initialize();
}

void Renderer::initialize_scenario()
{
    //If a file wasn't selected, return to main menu
    if (!load_scenario())
        return;

    start_shop();
    while (!game_ready_ && render_window_->isOpen()) // Render shop UI while player is shopping
    {
        sf::Event event;
        while (render_window_->pollEvent(event))
        {
            shop_ui_->execute_button_actions(*render_window_, event);
            if (event.type == sf::Event::Closed)
            {
                render_window_->close();
            }
        }
        render_window_->clear();
        shop_ui_->update();
        render_window_->draw(*shop_ui_);
        render_window_->display();
    }
    if (!render_window_->isOpen()) {
        return;
    }
    game_ = scenario_->generate_game();
    logs_ = std::make_shared<Game_Logs>(10);

    // store the pointer to the new level into the <tile_map_>, and
    // add it into the renderable,
    // also do this for every other object
    tile_map_ = std::make_shared<Tile_Map>( game_, 100 );
    r_map_ = std::make_shared<Render_Map>( tile_map_ );
    r_units_ = std::make_shared<Render_Units>( tile_map_ );
    r_buildings_ = std::make_shared<Render_Buildings>( tile_map_ );
    r_aux_ = std::make_shared<Render_Aux>( tile_map_ );

    // clear out the old renderables
    renderables_->clear();

    // add all the new renderables
    renderables_->add_drawable( r_map_ );
    renderables_->add_drawable( r_buildings_ );
    renderables_->add_drawable( r_units_ );
    renderables_->add_drawable( r_aux_ );


    if (!r_map_->load(map_text_path_)) {
        return;
    }
    if (!r_units_->load(unit_text_path_)) {
        return;
    }

    if (!r_buildings_->load(building_text_path_)) {
        return;
    }

    if (!r_aux_->load(aux_text_path_, text_font_path_)) {
        return;
    }

    game_->init_game();

    window_.get_game() = game_;

    start();
}

void Renderer::ready_game()
{
    game_ready_ = true;
}

void Renderer::start_main_screen() {
    MainScreen main_screen(*this, width_, height_);
    main_screen.start(*render_window_);
}

void Renderer::start()
{
    if (!r_map_->load(map_text_path_)) {
        return;
    }
    if (!r_units_->load(unit_text_path_)) {
        return;
    }

    if (!r_buildings_->load(building_text_path_)) {
        return;
    }

    if (!r_aux_->load(aux_text_path_, text_font_path_)) {
        return;
    }

    // game_->init_game();

    window_ = Rendering_Engine(game_, render_window_->getSize().x, render_window_->getSize().y);
    window_.render( width_, height_, *render_window_, *this, renderables_);
}

Game& Renderer::get_game() const {
    return *game_;
}
