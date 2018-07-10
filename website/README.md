## Website
To setup the website, install web assembly (see https://webassembly.org/getting-started/developers-guide/),
then run the following commands from the base folder of the project:
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<Path to Emscripten.cmake> -G "Unix Makefiles"
make
```
This will generate the necessary .wasm and .js files.