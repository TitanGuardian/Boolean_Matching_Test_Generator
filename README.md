# TestGenerator
This project created to generate tests for Boolean Matching problem in Logic Synthesis. Test looks like two circuit in Verlig language, which should be given as input for Bolean Matching solver.

## If you want to compile
First of all you have to compile abc library: https://github.com/berkeley-abc/abc
After, you have to copy compiled file "libabc.o" to ./abc

Also, you need installed boost program options library and readline development library.
If you need abc optimization, you need Python 3.

### Compilation:

cd TestGenerator
make

## After compilation
Now, you can run ./gentest . To view available options use -h --help arguments.

Simplest test:
./gentest -s 

After that command, you will see generated tests in ./out directory, also with .v files, you can see configuration of generated files, such as Boolean Function vector.
