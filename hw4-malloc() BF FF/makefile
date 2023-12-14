gcc -c -fPIC -o ff.o ff.c
gcc -shared -o ff.so ff.o
LD_PRELOAD=./ff.so ./main