#include <stdio.h>
#include <slibs/slibs.h>
#include <assert.h>

int main(int argc, char** argv) {
	assert(argc > 1);

	sl_string code = { .size = 0x200 };
	sl_read_file(argv[1], &code);

	for(int i = 0x200; i < code.size; i += 2) {
		printf("L%03X: %02X%02X\n", i, code.data[i] & 0xFF, code.data[i + 1] & 0xFF);
	}

	return 0;
}
