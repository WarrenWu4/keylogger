#include <iostream>

int main() {
#ifdef __linux__
    std::cout << "Linux detected\n";
#endif
    return 0;
}
