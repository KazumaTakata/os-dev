#include "string.h"

int strlen(char *str)
{
    int count = 0;
    while (str[count] != '\0')
    {
        count++;
    }
    return count;
}
void memset(char *str, uint8_t c, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        *(str + i) = c;
    }
}