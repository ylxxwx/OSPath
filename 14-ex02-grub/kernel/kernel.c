/* This will force us to create a kernel entry function instead of jumping to kernel.c:0x00 */
void dummy_test_entrypoint() {
}

void main() {
    char* video_memory = (char*) 0xb8000;
    *video_memory = 'Z';
    *(video_memory + 1) = 4;
    *(video_memory + 2) = 'Y';
    *(video_memory + 3) = 4;
    *(video_memory + 4) = 'X';
    *(video_memory + 5) = 4;
    *(video_memory + 6) = 'W';
    *(video_memory + 7) = 4;
}
