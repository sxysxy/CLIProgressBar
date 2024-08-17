# CLIProgressBar

A single python file to implementation progress bar in CLI. Cross-platform on Linux/MacOS/Windows. It's easy to embed to your project!

LICENSE: The MIT License. When you embed CLIProgressBar.py to your project, you are not required to attach the LICENSE file.

Usage:

```python
# Remember to import CLIProgressBar class

xs = list(range(100))
for x in CLIProgressBar(xs, desc="Description", unit='sample'):
    # do something with x...
    pass
```