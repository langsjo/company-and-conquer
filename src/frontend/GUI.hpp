#pragma once

#include <memory>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <../../libs/SFMLButton/include/sfmlbutton.hpp>
#include <limits>

#include "coordinates.hpp"
#include "auxiliary_renderable.hpp"
#include "inventory_ui.hpp"
#include "game_manager.hpp"



struct RectButtonGroup {
    bool isActive = false;
    std::vector<RectButton> buttons;

    virtual inline void clear_buttons() {
        buttons.clear();
    }
};

struct InventoryButtonGroup : public RectButtonGroup {

    int deactivateButtonIdx = -1;

    inline void clear_buttons() override {
        buttons.clear();
        deactivateButtonIdx = -1;
    }

    inline void clear_deactivate_button() {
        if (deactivateButtonIdx == -1) return;
        buttons.erase(buttons.begin() + deactivateButtonIdx);
        deactivateButtonIdx = -1;
    }

    inline bool deactivate_button_exists() const {
        return deactivateButtonIdx != -1;
    }

};


class Game;
class Map;
class Item;
class Unit;

class GUI : public Auxiliary_renderable
{
public:
    GUI() {};
    GUI(std::shared_ptr<Game_Manager> manager, size_t width, size_t height);

    /**
     * @brief load the font and initialize main buttons
     *
     */
    void initialize();

    /**
     * @brief update the drawable buttons of the GUI
     *
     */
    void update() override;


    /**
     * @brief handles pushing buttons and executes their associated action if pressed
     *
     * @param window 
     * @param event
     * @return bool true if a button was pushed, false if not
     */
    bool execute_button_actions(sf::RenderWindow& window, sf::Event& event);

    inline bool is_hovering_button() const { return is_hovering_a_button_; }

    /**
     * @brief tells the GUI what map coords were clicked on and handles the input accordingly
     *
     * @param y
     * @param x
     */
    void click_on_coords(size_t y, size_t x);

    /**
     * @brief returns a pointer to the active item, raw pointer since no ownership
     *
     * @return const Item*
     */
    const Item* get_active_item() const;

    void deselect_unit();
    void undo_action();
    void next_turn();

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    bool selected_unit_in_active_team() const;

    // These update/initialize the associated buttons
    void initialize_main_buttons();
    void update_inventory();

    // Draws all buttons in given group
    void draw_button_group(sf::RenderTarget& target, const RectButtonGroup& group) const;

    std::vector<RectButton*> get_all_buttons();


public:
    bool are_logs_active = true;
private:
    std::shared_ptr<Game_Manager> game_manager_;
    Map* map_;

    bool selected_unit_changed_ = false;

    std::shared_ptr<const Item> active_item;
    bool active_item_changed_ = false;
    sf::Vector2f active_item_pos_;

    std::unique_ptr<sf::Font> font_ = std::make_unique<sf::Font>();
    sf::Vector2f inventory_buttons_start_pos_;
    sf::Vector2f main_buttons_start_pos_;
    sf::Vector2f main_buttons_pos_relative_to_inv_buttons_ = {-30, -80};

    // the inventory backround
    std::shared_ptr<Inventory_UI> r_inv_;

    size_t width_ = 0;
    size_t height_ = 0;
    
    bool is_hovering_a_button_ = false;

    RectButtonGroup main_buttons_;
    InventoryButtonGroup inventory_buttons_;
};
