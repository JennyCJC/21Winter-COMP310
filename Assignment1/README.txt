
Please put all files including: shell.c shellmemory.c interpreter.c shell.h shellmemory.h interpreter.h test.txt testfile.txt under the same directory. 

In order to compile all the files, run the following command in terminal:
gcc -Wall shell.c shellmemory.c interpreter.c -o mysh

In order to run the executable, run the following command in terminal:
./mysh

Expected outcome when running "./mysh < testfile.txt" :

Welcome to Jenny's Shell!
Version 1.0 Created January 2021
$Valid commands are:
quit
set
print
run
$Unknown command.
$$12
$$Jenny
$$15
$Unknown command.
$Variable does not exist
$Valid commands are:
quit
set
print
run
Script not found.
hurray!
