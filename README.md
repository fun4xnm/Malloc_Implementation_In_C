Malloc_Implementation
=====================
gcc -g -Wall -O0 -fPIC -shared malloc.c -o libyamalloc.so

gcc test.c -L . -lmymalloc

x/40xw ptr
