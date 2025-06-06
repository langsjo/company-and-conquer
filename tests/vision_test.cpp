#include <memory>

#include "map.hpp"
#include "coordinates.hpp"

static std::unordered_map<char, Terrain> terrains;
static size_t width = 20;
static size_t height = 10;


void change_terrain1(Map& a_map) {
  std::cout << "Enter coordinates in form 'x y'" << std::endl;
  int x, y;
  std::cin >> x >> y;
  if (std::cin.fail()) {return;}

  char terrain_type;
  std::cout << "Enter terrain character representative (. or # or -)" << std::endl;
  std::cin >> terrain_type;
  if (std::cin.fail()) {return;}

  switch (terrain_type) {
    case '.':
      a_map.update_terrain(terrain_type, y, x);
      return;
    case '#':
      a_map.update_terrain(terrain_type, y, x);
      return;
    case '-':
      a_map.update_terrain(terrain_type, y, x);
      return;
    default:
      return;
  }
}

void print_vision(Map& map) {
  while (true) {
    std::cout << "Enter coordinates in form 'x y'" << std::endl;
    size_t x, y;
    std::cin >> x >> y;
    if (std::cin.fail()) {std::cin.clear();return;}

    std::vector<coordinates<size_t>> visibility = map.tiles_unit_sees({x, y}, 5);

    for (size_t y = 0; y < height; ++y) {
      for (size_t x = 0; x < width; ++x) {
        if (std::find_if(visibility.begin(), visibility.end(), [x, y](const coordinates<size_t>& coords) {
          return (coords.x == x && coords.y == y);
        }) != visibility.end()) {
          std::cout << '@';
        } else {
          std::cout << map.get_terrain(y, x)->get_repr();
        }
      }
      std::cout << '\n';
    }
    std::cout << std::endl;
    return;
  }
}


int vision_test() {

  Map map(width, height);

  Terrain background('.');
  Terrain wall = Terrain('#', false, false, false, false);
  Terrain swamp('-', 3);
  terrains[background.get_repr()] = background;
  terrains[wall.get_repr()] = wall;
  terrains[swamp.get_repr()] = swamp;

  while (true) {
    std::cout << "A to change terrain, M to calculate movement, Q to quit" << std::endl;
    char input;
    std::cin >> input;
    if (std::cin.fail()) {std::cin.clear(); continue;}

    switch (input) {
      case 'A':
        change_terrain1(map);
        map.print_map();
        break;
      case 'M':
        print_vision(map);
        break;
      case 'Q':
        return EXIT_SUCCESS;
      default:
        std::cin.clear();
    }
  }
}

