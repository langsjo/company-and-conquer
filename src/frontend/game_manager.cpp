#include "game_manager.hpp"
#include "tile_map.hpp"

Game_Manager::Game_Manager(std::weak_ptr<Game> game, std::weak_ptr<Tile_Map> tile_map) : game_(game), tile_map_(tile_map) {}

Map& Game_Manager::get_map() {
    return game_.lock()->get_map();
}

bool Game_Manager::selected_valid_unit() const {
    return selected_unit_coords_ != invalid_coord && selected_unit_ptr_ != nullptr;
}

//NOTE: call when changing turn or deselecting unit
void Game_Manager::deselect_unit() {
    selected_unit_coords_ = invalid_coord;
    selected_unit_ptr_ = nullptr;

    coords_selected_unit_can_move_to_.clear();
    coords_selected_unit_can_shoot_to_.clear();
}

//NOTE: call when selecting unit
bool Game_Manager::select_unit_on_coords(const coordinates<size_t>& origin) {
    std::shared_ptr<Game> game = game_.lock();
    if (!game->get_map().has_unit(origin) || !(game->game_started())) return false;

    Unit* unit_ptr = game->get_map().get_unit(origin);
    if (unit_ptr->is_dead() || game->get_unit_team_id(unit_ptr->get_id()) != game->get_active_team()->get_id()) return false;

    selected_unit_coords_ = origin;
    selected_unit_ptr_ = unit_ptr;

    // Calculate these coordinates only once per selection of unit
    coords_selected_unit_can_move_to_ = game->get_map().possible_tiles_to_move_to3(origin, unit_consts.move_range);
    coords_selected_unit_can_shoot_to_ = game->get_map().tiles_can_shoot_on(origin, unit_consts.visual_range);

    return true;
}

const coordinates<size_t>& Game_Manager::selected_unit_coords() const { return selected_unit_coords_; }

const std::vector<coordinates<size_t>>& Game_Manager::selected_unit_possible_movements() const {
    return coords_selected_unit_can_move_to_;
}

Unit* Game_Manager::selected_unit_ptr() {
    return selected_unit_ptr_;
}

bool Game_Manager::enqueue_movement_action(const coordinates<size_t>& target) {
    if (!selected_valid_unit() || selected_unit_ptr_->has_moved || !(can_selected_unit_move_to(target))) return false;

    std::shared_ptr<Game> game = game_.lock();
    std::shared_ptr<MovementAction> next_action = std::make_shared<MovementAction>(selected_unit_coords_,target,*selected_unit_ptr_);
    if (!(game->add_action(next_action, game->get_active_team()->get_id()))) return false;

    deselect_unit();
    return true;
}

bool Game_Manager::enqueue_item_action(coordinates<size_t> target, const Item* action_item) {
    if (!selected_valid_unit()) return false;
    if (!can_selected_unit_use_item_to(target)) return false;
    if (selected_unit_ptr_->has_added_action) return false;
    assert(action_item != nullptr && "Gave nullptr to enqueue_item_action");

    std::shared_ptr<Action> next_action = action_item->get_action(target, *selected_unit_ptr_);
    if (!(game_.lock()->add_action(next_action,game_.lock()->get_active_team()->get_id()))) return false;

    deselect_unit();
    return true;
}

bool Game_Manager::undo_action() {
    std::shared_ptr<Game> game = game_.lock();
    // If an action was undone
    if (game->undo_action(game->get_active_team()->get_id())) {
        deselect_unit();
        return true;
    }

    return false;
}

void Game_Manager::next_turn() {
    std::shared_ptr<Game> game = game_.lock();
    game->next_turn();
    deselect_unit();
}

void Game_Manager::cycle_units(int window_width, int window_height) {
    std::vector<Unit*> active_team_alive_units = game_.lock()->get_active_team()->get_alive_units();
    unit_cycle_idx_ = (unit_cycle_idx_ + 1) % active_team_alive_units.size();
    
    Unit* next_unit_ptr = active_team_alive_units[unit_cycle_idx_];
    coordinates<size_t> next_unit_coords = get_map().get_unit_location(next_unit_ptr);

    select_unit_on_coords(next_unit_coords);
    tile_map_.lock()->center_at(next_unit_coords, window_width, window_height);
}

std::string Game_Manager::get_action_info(const coordinates<size_t>& potential_target, const Item* action_item) {

    std::stringstream ss;
    get_tile_info(ss,potential_target);
    //Only add the following if a unit is selected
    if (selected_valid_unit()) {
        ss << selected_unit_ptr_->get_name() << "\n";
        get_movement_action_info(ss,potential_target);
        get_item_action_info(ss,potential_target,action_item);
    }
    return ss.str();
}

void Game_Manager::get_movement_action_info(std::stringstream& info_stream, const coordinates<size_t>& potential_target) {
    if (!selected_valid_unit()) return;
    if (selected_unit_ptr_->has_moved) {
        info_stream << "has already moved\n";

    } else if (can_selected_unit_move_to(potential_target)) {
        info_stream << "can move here\n";

    } else {
        info_stream << "cannot move here\n";
    }
    return;
}

// NOTE: update these strings
void Game_Manager::get_item_action_info(std::stringstream& info_stream, const coordinates<size_t>& potential_target, const Item* action_item) {
    if (!selected_valid_unit()) return;
    if (selected_unit_ptr_->has_added_action) { info_stream << "item action already queued\n"; return; }
    if (action_item == nullptr) { return; }
    bool can_use = can_selected_unit_use_item_to(potential_target); 
    if (can_use) { info_stream << action_item->get_info(selected_unit_coords_, potential_target); }
    else { info_stream << "Can not use item here"; }
    return;
}

void Game_Manager::get_tile_info(std::stringstream& info_stream, const coordinates<size_t>& potential_target) {
    if (!are_valid_coords(potential_target) || !(game_.lock()->game_started())) return;
    info_stream << potential_target << "\n";
    Unit* target_unit_ptr = game_.lock()->get_map().get_unit(potential_target);
    Team* active_unit_team = game_.lock()->get_active_team();
    if (target_unit_ptr == nullptr) return;
    if (active_unit_team->get_unit(target_unit_ptr->get_id()) == nullptr) {
        info_stream << "Enemy ";
    } else {
        info_stream << "Friendly ";
    }
    info_stream << target_unit_ptr->get_name() << ", HP: " << target_unit_ptr->get_hp() << "\n";
}

bool Game_Manager::can_selected_unit_move_to(const coordinates<size_t>& potential_target) const {
    if (!selected_valid_unit() || !are_valid_coords(potential_target)) return false;
    Map& map = game_.lock()->get_map();
    if (!map.can_move_to_coords(potential_target)) return false;

    auto it = std::find(coords_selected_unit_can_move_to_.begin(), coords_selected_unit_can_move_to_.end(), potential_target);
    return it != coords_selected_unit_can_move_to_.end();
}

bool Game_Manager::can_selected_unit_use_item_to(const coordinates<size_t>& potential_target) const {
    if (!selected_valid_unit() || !are_valid_coords(potential_target)) return false;

    auto it = std::find(coords_selected_unit_can_shoot_to_.begin(), coords_selected_unit_can_shoot_to_.end(), potential_target);
    return it != coords_selected_unit_can_shoot_to_.end();
}

bool Game_Manager::are_valid_coords(const coordinates<size_t>& coords) const {
    Map& map = game_.lock()->get_map();
    return map.are_valid_coords(coords);
}
