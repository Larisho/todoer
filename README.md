# TODOER

## Building
To build, just run `make`. This project uses `clang` by default, but it can also be compiled on `gcc`.

## The point
The point of this is to have a utility that tracks all my TODOs in my software projects. This would only ever be a backend to the tracking of TODOs.

## Improvements
- [ ] Find TODOs in more than just C-based languages (currently searching for `// TODO`).
- [ ] Exclude directory path as opposed to by directory name
- [ ] Exclude directories based on regex
- [ ] Exclude files by name and regex
- [ ] Abbreviate lines that are longer than X characters (or something like this...)

## Alterior motives
This project has also been created as a way to compare C with Rust in performance and ease of development. Eventually, I will write a Rust version of this tool and will measure some benchmarks between both programs.
