gcc -c src/pcb.c
gcc -c src/cpu.c
gcc -c src/linkedList.c
gcc -c src/program.c

gcc -o commander.exe linkedList.o program.o cpu.o pcb.o src/commander.c
gcc -o manager.exe linkedList.o program.o cpu.o pcb.o src/manager.c

./commander.exe