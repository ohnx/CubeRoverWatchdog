CFLAGS+=-Iinclude/ -g -O0 -D__HOST_DEBUG -Wall -Werror

test_buffer: objs/test_buffer
	./objs/test_buffer

objs/test_buffer: src/buffer_test.c
	gcc $(CFLAGS) src/buffer_test.c -o objs/test_buffer



