#include "renderer.hpp"

//Use WASD to move map.
//Clicking tile will print the coordinates of that tile.
//Left click unit to make that unit priority unit. After that you can move to another tile by pressing right click on that tile.
//Press space to end turn.
//Log messages will appear in terminal.
void rendering_engine_test() {
    Renderer renderer(900,900);
    renderer.initialise_level(0);
    Game& test_game = renderer.get_game();
    Map& test_map = test_game.get_map();
    Team team1;
    Team team2;
    Unit unit1("0");
    Unit unit2("1");
    Unit unit3("2");
    team1.add_unit(unit1);
    team1.add_unit(unit3);
    team2.add_unit(unit2);
    test_game.add_team(team1);
    test_game.add_team(team2);
    Team& t1 = test_game.get_teams()[0];
    Unit* u1 = &t1.get_units()[0];
    u1->add_item(std::make_shared<Weapon>("gun1",100,100,0));
    u1->add_item(ConstItem::turret_barrel);
    u1->add_item(ConstItem::turret_legs);
    Unit* u3 = &t1.get_units()[1];
    Team& t2 = test_game.get_teams()[1];
    Unit* u2 = &t2.get_units()[0];
    u2->add_item(std::make_shared<Weapon>("gun2",100,100,0));
    test_map.add_unit(0,0,u1);
    test_map.add_unit(2, 2, u3);
    test_map.add_unit(5,5,u2);

    test_game.set_ai_controlled_team(t2.get_id());
    renderer.start();
}
