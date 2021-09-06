Tiny LISP interpreter through [Build Your Own Lisp](https://buildyourownlisp.com/) tutoral.

## How to build

On Windows, try [MinGW](https://www.mingw-w64.org/) to install the compiler.  
On Mac, run `xcode-select --install` to install build tools.  
On Linux, install `build-essential` package. Try running `sudo apt-get install build-essential` on Ubuntu or Debian, `sudo yum groupinstall "Development Tools"` on CentOS or Fedora.

```bash
# Build using cc and run the program.
$ cc -std=c99 -Wall *.c -ledit -lm -o build/parsing && ./build/parsing
```

Try `gcc` if you can't find `cc`.
