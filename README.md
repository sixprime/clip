# CLIP

**C**ommand-**LI**ne **P**arser. Inspired by the Rust and C++ versions of Structopt and Clap.

Part of the [Convoy](https://github.com/sixprime/convoy) project.

## Install

```bash
D:\dev\>git clone --recurse-submodules https://github.com/sixprime/clip.git   
Cloning into 'clip'...
remote: Enumerating objects: 9, done.
remote: Counting objects: 100% (9/9), done.
remote: Compressing objects: 100% (7/7), done.
remote: Total 9 (delta 1), reused 4 (delta 0), pack-reused 0
Receiving objects: 100% (9/9), done.
Resolving deltas: 100% (1/1), done.
Submodule 'external/visit_struct' (https://github.com/sixprime/visit_struct.git) registered for path 'external/visit_struct'
Cloning into 'D:/dev/clip/external/visit_struct'...
remote: Enumerating objects: 837, done.        
remote: Counting objects: 100% (32/32), done.
remote: Compressing objects: 100% (22/22), done.        
remote: Total 837 (delta 11), reused 17 (delta 6), pack-reused 805        
Receiving objects: 100% (837/837), 236.01 KiB | 856.00 KiB/s, done.
Resolving deltas: 100% (526/526), done.
Submodule path 'external/visit_struct': checked out '93271681c973cc4cc33d79f110f12f2b09bc2d6a'
```

## Build

Uses `ninja` : https://ninja-build.org/.
Only tested with `clang++`, and requires C++20.

```bash
D:\dev\clip>ninja  
[2/2] clang++ -Lbuild build/tests/simple.o -o clip.exe
```

## Usage

```bash
$ clip.exe 0b1111 3.1415161718 "my beautiful string" --id 42
command_line_arguments = { x = 15, f = 3.14151621, str = "my beautiful string", id = 42 (is set) }
```

### Notes

External dependencies will be handled differently in the future once the Convoy package manager is more mature. For now, we have to include them in the project as git submodules.
