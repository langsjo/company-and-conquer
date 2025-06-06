#ifndef TILE_MAP_HPP
#define TILE_MAP_HPP

#include <memory>
#include <utility>

#include "game.hpp"
#include "coordinates.hpp"


/**
 * @brief This class will help to render each map layer such as tiles and units together.
 */
class Tile_Map {
public:
    Tile_Map(std::shared_ptr<Game>& game, int tileDim);
    Tile_Map(std::shared_ptr<Game>& game, std::pair<float, float> x0y0, int tileDim);

    /**
     * @brief Checks of a tile in certain coords is show or hidden due to fog of war. Uses visible_coords from game.
     */
    bool is_tile_drawn(size_t x, size_t y) const;
    bool is_tile_drawn(const coordinates<size_t>& coords) const;

    bool fog_of_war = true; //Can be used to toggle fog of war. Only for developing.

    /**
     * @brief Moves all tiles.
     * 
     * @param x How much the x coordinate of a tile will be changed.
     * @param y How much the y coordinate of a tile will be changed.
     */
    void move(float x, float y);

    /**
     * @brief Can be used to move map so coords are in the center of the screen.
     */
    void center_at(const coordinates<size_t>& coords, int window_width, int window_height); 

    /**
     * @brief Transforms matrix (indexes?) into pixels coordinates.
     */
    std::pair<int, int> get_tile_coords(int x, int y) const;
    std::pair<int, int> get_tile_coords(const coordinates<size_t>& coords) const;

    /**
     * @brief Transfroms pixel coordinates into matrix (indexes?)
     */
    coordinates<size_t> get_map_coords(int pixel_x, int pixel_y) const;

    /**
     * @brief Checks if certain pixel or tile coordinates are inside or outside of game map.
     */
    bool is_inside_map_tile(int x, int y) const;
    bool is_inside_map_tile(const coordinates<size_t>& coords) const;
    bool is_inside_map_pixel(int pixel_x, int pixel_y) const;

    int get_TileDim() const;

    std::pair<float,float> get_x0y0() const;

    Map& get_map() const;

    std::weak_ptr<Game> get_game() const;
    
    void set_game( std::shared_ptr<Game> game );

private:
    std::shared_ptr<Game> game_;
    std::pair<float, float> x0y0_; //Top right pixel coordinate of the game map.
    int tileDim_; //The pixel width and height of each tile.
};

#endif
