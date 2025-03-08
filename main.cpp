#include <iostream>

#ifdef __linux__
#endif

int main() {
#ifdef __linux__
    std::cout << "Linux\n";
#endif
    return 0;
}
