#include "src/map.hpp"
#include <iostream>

int main() {
    sjtu::map<int, int> m;
    m[1] = 10;
    m[2] = 20;
    std::cout << "Inserted" << std::endl;
    std::cout << "m[1] = " << m[1] << std::endl;
    std::cout << "m[2] = " << m[2] << std::endl;
    std::cout << "Size: " << m.size() << std::endl;
    for (auto it = m.begin(); it != m.end(); ++it) {
        std::cout << it->first << " -> " << it->second << std::endl;
    }
    return 0;
}
