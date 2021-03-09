# CPP Texas Solver

Cpp version of [TexasHoldemSolverJava](https://github.com/bupticybee/TexasHoldemSolverJava) faster on turn and river but slower on flop.

Embedded with python iterfaces.

## How to use?
```bash
mkdir build
cd build
cmake ..
make install
./test
```

## Windows
build with visual studio 2019
```
mkdir vsbuild
cd vsbuild
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake install
test.exe
```