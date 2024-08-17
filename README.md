# CLIProgressBar

A single C++/python file to implementation progress bar in CLI. Cross-platform on Linux/MacOS/Windows. It's easy to embed to your project!

LICENSE: The MIT License. When you embed CLIProgressBar.py to your project, you are not required to attach the LICENSE file.

Example Usage:

Python:
```python
# Remember to import CLIProgressBar class

xs = list(range(100))
for x in CLIProgressBar(xs, desc="Description", unit='sample'):
    # do something with x...
    pass
```

C++:
```cpp
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
```