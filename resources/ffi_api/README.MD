## Usage

### C FFI API

You can easily call the library using C FFI compatible languages like Rust, Python, Dart, etc.

For example, if you are using Python on Windows, an easy way to load the library and call the api function is as follows:

```python
from ctypes import *
api_library = CDLL('api.dll') # python script in same directory as library

api_library.api(b"resources/text/commandline_sample_input.txt", b"./resources", b"holdem") # should just work
# verify that it generates a file with the solution
```

Linux users should use this command:

```python
api_library = CDLL('api.so') 
```

And MacOs:

```python
api_library = CDLL('api.dylib') 
```

PLEASE, this is just an example. Use the python bindings provided by Texas Solver as it has more features and flexibility.

After the solving process is done, a ```output_result.json``` file should be generated. It contains the optimal strategy that was calculated.

The input file, above mentioned ```resources/text/commandline_sample_input.txt``` contains parameters controls the tree building and solving process. A typical such file look like this:

```text
set_pot 50
set_effective_stack 200
set_board Qs,Jh,2h
set_range_ip AA,KK,QQ,JJ,TT,99:0.75,88:0.75,77:0.5,66:0.25,55:0.25,AK,AQs,AQo:0.75,AJs,AJo:0.5,ATs:0.75,A6s:0.25,A5s:0.75,A4s:0.75,A3s:0.5,A2s:0.5,KQs,KQo:0.5,KJs,KTs:0.75,K5s:0.25,K4s:0.25,QJs:0.75,QTs:0.75,Q9s:0.5,JTs:0.75,J9s:0.75,J8s:0.75,T9s:0.75,T8s:0.75,T7s:0.75,98s:0.75,97s:0.75,96s:0.5,87s:0.75,86s:0.5,85s:0.5,76s:0.75,75s:0.5,65s:0.75,64s:0.5,54s:0.75,53s:0.5,43s:0.5
set_range_oop QQ:0.5,JJ:0.75,TT,99,88,77,66,55,44,33,22,AKo:0.25,AQs,AQo:0.75,AJs,AJo:0.75,ATs,ATo:0.75,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,KQ,KJ,KTs,KTo:0.5,K9s,K8s,K7s,K6s,K5s,K4s:0.5,K3s:0.5,K2s:0.5,QJ,QTs,Q9s,Q8s,Q7s,JTs,JTo:0.5,J9s,J8s,T9s,T8s,T7s,98s,97s,96s,87s,86s,76s,75s,65s,64s,54s,53s,43s
set_bet_sizes oop,flop,bet,50
set_bet_sizes oop,flop,raise,60
set_bet_sizes oop,flop,allin
set_bet_sizes ip,flop,bet,50
set_bet_sizes ip,flop,raise,60
set_bet_sizes ip,flop,allin
set_bet_sizes oop,turn,bet,50
set_bet_sizes oop,turn,raise,60
set_bet_sizes oop,turn,allin
set_bet_sizes ip,turn,bet,50
set_bet_sizes ip,turn,raise,60
set_bet_sizes ip,turn,allin
set_bet_sizes oop,river,bet,50
set_bet_sizes oop,river,donk,50
set_bet_sizes oop,river,raise,60,100
set_bet_sizes oop,river,allin
set_bet_sizes ip,river,bet,50
set_bet_sizes ip,river,raise,60,100
set_bet_sizes ip,river,allin
set_allin_threshold 0.67
build_tree
set_thread_num 8
set_accuracy 0.5
set_max_iteration 200
set_print_interval 10
set_use_isomorphism 1
start_solve
set_dump_rounds 2
dump_result output_result.json
```

Modify the file to control the game tree details and parallelism, etc. By default the input file contains a flop situation where game tree is constructed using 1bet/raise+allin in flop and turn, using 2bet/raise+allin in river.

Check out full python implementation in the 'resources/ffi_api' folder, as it contains a more detailed explanation

If you wish to see other language implementations then ... (create a github issue?)