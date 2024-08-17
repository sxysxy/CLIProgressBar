#include "CLIProgressBar.hpp"
#include <chrono>
#include <vector>
#include <thread>

int main() {
    std::vector<int> v;
    for(int i = 0; i < 100; i++) {
        v.push_back(i+1);
    }
    int s = 0;
    CLIProgressBar<std::vector<int>::iterator> bar(v.begin(), v.end(), "test", "it");
    // The 3-rd argument could be a std::ostream, for example, std::cout, it also works!
    //CLIProgressBar<std::vector<int>::iterator> bar(v.begin(), v.end(), std::cout, "test", "it");  
    bar.run([&](int i) {
        s += i;
        std::this_thread::sleep_for(std::chrono::nanoseconds(int(1e7)));
    });
    printf("%d\n", s);
    return 0;
}