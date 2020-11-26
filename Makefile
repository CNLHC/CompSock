target=libhooks.so

OBJS=hooks.o compress.o

CFLAGS=-D_GNU_SOURCE -g

LINK=  -ldl -lz

all: ${target}

.PHONY: clean

clean:
	rm -rf *.o *.so

${target}: ${OBJS}
	gcc -g -shared -o ${target} -fPIC  $^ -ldl -lz

test-compress: compress.o test_compress.o
	gcc -o $@  $^ -lz -ldl
	
%.o:%.c
	gcc ${CFLAGS} -fPIC $< -c

