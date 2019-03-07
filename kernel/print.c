#include "print.h"
#include "string.h"

void print_ch(char c, int offset)
{
    char *video_memory = (char *)VIDEO_MEMORY_ADD;
    *(video_memory + (offset * 2)) = c;
    *(video_memory + (offset * 2 + 1)) = 0x1F;
}

void print_str(char *str, int row, int col)
{
    int offset = row * 80 + col;
    for (int i = 0; i < strlen(str); i++)
    {
        print_ch(str[i], i + offset);
    }
}
