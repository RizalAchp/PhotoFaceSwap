# PHOTOFACESWAP

just simple project for me to learn machinelearning with opencv in c++

## Build From Source
Make sure OpenCV and CMake (c/c++ buildsystem).

#### build on linux
- clone the project and change directory to project dir
```sh
git clone https://github.com/RizalAchp/PhotoFaceSwap.git && cd PhotoFaceSwap
```
- generate build configuration
```sh
cmake -B ${PWD}/build -S ${PWD}
```
- build the project
```sh
cmake --build ${PWD}/build --config Release --target all -- -j8
```
#### build on windows
- clone or download zip file project and change directory to project dir
```ps1
git clone https://github.com/RizalAchp/PhotoFaceSwap.git && cd PhotoFaceSwap
```
- generate and build project with bat script
```ps1
.\build.bat
```


## Running the Executeable

- executeable in ./build/bin/PhotoFaceSwap or PhotoFaceSwap.exe if in windows
- run for cli pass `-h` or `--help` flag for command list and instructions
```sh
./build/bin/PhotoFaceSwap -h
```
- or in windows
```ps1
.\build\bin\PhotoFaceSwap.exe -h
```

- run for gui just run the proggrams, without flags
```sh
./build/bin/PhotoFaceSwap
```
- or in windows
```ps1
.\build\bin\PhotoFaceSwap.exe
```
