#pragma once

#include <optional>
#include <string>
#include <vector>
#include <cstdint>
#include <array>
#include <algorithm>
#include <iterator>
#include <bitset>
#include <random>
#include <functional>

#include "helper_tools.hpp"
#include "coordinates.hpp"
#include "const_terrains.hpp"



class Terrain
{
    private:
        // indeces for accessing <terrain_properties_>
        static const size_t can_shoot_through_ = 0;
        static const size_t can_see_through_ = 1;
        static const size_t can_walk_through_ = 2;
        static const size_t can_build_in_ = 3;

        char character_repr_;
       
        size_t movement_cost_ = 1;
        std::bitset<4> terrain_properties_ = std::bitset<4>{}.set(); // special version of std::vector that should be implemented as a bitset


    public:

        Terrain(char repr = '.') : character_repr_(repr) {}

        Terrain(char repr, bool can_shoot, bool can_see, bool can_walk, bool can_build) : character_repr_(repr) 
        { 
            terrain_properties_[can_shoot_through_] = can_shoot;
            terrain_properties_[can_see_through_] = can_see;
            terrain_properties_[can_walk_through_] = can_walk;
            terrain_properties_[can_build_in_] = can_build;
        }

        Terrain( char repr, bool can_shoot, bool can_see, bool can_walk, bool can_build, size_t movement_cost ) : character_repr_( repr ), movement_cost_( movement_cost ) 
        { 
            terrain_properties_[can_shoot_through_] = can_shoot;
            terrain_properties_[can_see_through_] = can_see;
            terrain_properties_[can_walk_through_] = can_walk;
            terrain_properties_[can_build_in_] = can_build;
        }
        Terrain( char repr, size_t movement_cost ) {
            character_repr_ = repr;
            movement_cost_ = movement_cost;
         }


        int32_t texture() const {
            return ConstTerrain::get_texture_idx_from_char(character_repr_);
        }

        constexpr size_t movement_cost() const { return movement_cost_; }

        void set_movement_cost(size_t cost) {movement_cost_ = cost;}

        bool can_move_to() const
        {
            return ( terrain_properties_[can_walk_through_] );
        }

        bool can_see_through() const
        {
            return ( terrain_properties_[can_see_through_] );
        }

        bool can_shoot_through() const
        {
            return ( terrain_properties_[can_shoot_through_] );
        }

        bool can_build_on() const {
            return terrain_properties_[can_build_in_];
        }

        char get_repr() const {return character_repr_;}

        

};
