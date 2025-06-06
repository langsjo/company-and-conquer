#include "tile_map.hpp"

Tile_Map::Tile_Map(std::shared_ptr<Game>& game, int tileDim) : tileDim_(tileDim) {
    game_ = game;
    return;
}

Tile_Map::Tile_Map(std::shared_ptr<Game>& game, std::pair<float, float> x0y0, int tileDim) : x0y0_(x0y0), tileDim_(tileDim) {
    game_ = game;
    return;
}

bool Tile_Map::is_tile_drawn(size_t x, size_t y) const {
    return is_tile_drawn(coordinates<size_t>(x, y));
}

bool Tile_Map::is_tile_drawn(const coordinates<size_t>& coords) const {
    const std::vector<coordinates<size_t>>& visible_coords = game_->get_visible_tiles();
    auto coord_it = std::find(visible_coords.begin(), visible_coords.end(), coords);
    return ((!fog_of_war) || (coord_it != visible_coords.end()));
}

void Tile_Map::move(float x, float y) {
    x0y0_.first = x0y0_.first + x;
    x0y0_.second = x0y0_.second + y;
    return;
}

std::pair<int,int> Tile_Map::get_tile_coords(int x, int y) const {
    std::pair<int,int> crds( ( tileDim_ * x ) + x0y0_.first , ( tileDim_ * y ) + x0y0_.second );
    return crds;
}

std::pair<int,int> Tile_Map::get_tile_coords(const coordinates<size_t>& coords) const {
    return get_tile_coords( coords.x , coords.y );
}

coordinates<size_t> Tile_Map::get_map_coords(int pixel_x, int pixel_y) const {
    size_t x = std::floor(( pixel_x - x0y0_.first ) / tileDim_);
    size_t y = std::floor(( pixel_y - x0y0_.second ) / tileDim_);
    return coordinates<size_t>( x , y );
}

bool Tile_Map::is_inside_map_tile(int x, int y) const {
    return get_map().are_valid_coords(y, x);
}

bool Tile_Map::is_inside_map_tile(const coordinates<size_t>& coords) const {
    return is_inside_map_tile( coords.x , coords.y );
}

bool Tile_Map::is_inside_map_pixel(int pixel_x, int pixel_y) const {
    return (pixel_x >= x0y0_.first && pixel_x < (get_map().width() * tileDim_ + x0y0_.first)) && (pixel_y >= x0y0_.second && pixel_y < (get_map().height() * tileDim_ + x0y0_.second));
}


void Tile_Map::center_at(const coordinates<size_t>& coords, int window_width, int window_height) {
    float window_center_x = window_width / 2;
    float window_center_y = window_height / 2;
    x0y0_ = std::pair<int,int>(window_center_x - tileDim_/2 - coords.x * tileDim_,window_center_y - tileDim_/2 - coords.y * tileDim_);
}

int Tile_Map::get_TileDim() const {
    return tileDim_;
}
std::pair<float,float> Tile_Map::get_x0y0() const {
    return x0y0_;
}

Map& Tile_Map::get_map() const {
    return game_->get_map();
}

std::weak_ptr<Game> Tile_Map::get_game() const {
    return game_;
}

void Tile_Map::set_game( std::shared_ptr<Game> game ) {
    game_ = game;
    return;
}


