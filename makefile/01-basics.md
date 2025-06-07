# Getting Started with Makefiles

## Why Makefiles Exist
Makefiles help determine which parts of a large program need recompilation. Primarily used for:
- C/C++ compilation
- Can be used for any task requiring execution based on file changes

### Example Dependency Graph
If any file's dependencies change, the file gets recompiled.

## Alternatives to Make
| Language    | Build Tools                     |
|-------------|---------------------------------|
| C/C++       | SCons, CMake, Bazel, Ninja      |
| Java        | Ant, Maven, Gradle              |
| Go/Rust/TS  | Language-specific tools         |
| Interpreted | Not needed (Python, Ruby, JS)   |

## Make Versions
- Focus on GNU Make (standard on Linux/MacOS)
- Compatible with versions 3 and 4

## Basic Example
```makefile
hello:
    echo "Hello, World"
```
Run with `make` command.

**Note**: Must use TABs for indentation, not spaces.

## Makefile Syntax
```makefile
targets: prerequisites
    command
    command
    command
```

## The Essence of Make
- Targets are tied to files
- Commands run if:
  - Target file doesn't exist, OR
  - Prerequisites are newer than target

### Compilation Example
```makefile
blah: blah.c
    cc blah.c -o blah
```
Make uses filesystem timestamps to determine what needs rebuilding.

## Multi-Target Example
```makefile
blah: blah.o
    cc blah.o -o blah

blah.o: blah.c
    cc -c blah.c -o blah.o

blah.c:
    echo "int main() { return 0; }" > blah.c
```
Build process flows through dependencies.

## Clean Target
```makefile
some_file:
    touch some_file

clean:
    rm -f some_file
```
**Note**: `clean` isn't special - just convention.

## Variables
```makefile
files := file1 file2
some_file: $(files)
    echo "Look at: " $(files)
    touch some_file
```

### Variable Styles
```makefile
x := value

all:
    echo $(x)    # Recommended
    echo ${x}    # Also works
    echo $x      # Works but bad practice
```

## Important Notes
- Quotes have no special meaning in Make variables
- Always use TABs for commands
- First target is the default when running `make`
