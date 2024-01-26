emulator: emulator.c
	gcc emulator.c -o emulator -Iinclude -g -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
inspector: inspector.c
	gcc inspector.c -o inspector -Iinclude -g
hello: hello.asm
	c8asm hello.asm -o hello
run: hello emulator
	./emulator hello
