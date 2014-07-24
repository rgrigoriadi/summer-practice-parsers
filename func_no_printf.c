#include "stdio.h"

int find_start(char*str, int last_end);
int find_end(char* str, int start);
int str_cmp(char* big_str, int start, int end, char* little_str);
int is_digit(char c);
int get_num(char* str, int max_len, int* num_len);
void get_range(char* str, int start, int pos, int* ref_start, int* ref_end, int* ref_bit);

int main(int argc, char** argv)
{
    const char* test_string = "  1..4,  3..12, 16..19, 21, trace,!9..11";
    parse(test_string);
    return 0;
}

int parse(char* str)
{
    printf("input string = '%s'\n", str);

    int start = 0, end = 0;
    int result = 0;

    while (1)
    {
        start = find_start(str, end);
        end = find_end(str, start);

        int bit = 1;
        if (str[start] == '!')
        {
            bit = 0;
            start++;
        }

        int bit_start = -1, bit_end = -2;

        get_range(str, start, end, &bit_start, &bit_end, &bit);
        int i;
        for (i = bit_start; i <= bit_end; i++)
        {
            int shift = 31 - i;
            int mask = 1 << shift;
            if (bit != 0)
                result |= mask;
            else
                if ((result & mask) >> shift)
                    result ^= mask;
        }

        if (str[end] == '\0')
            break;
            
    }
    
    printf("result (%u) = ", result);
    int shift;
    for (shift = 31; shift >= 0; shift --)
    {
        int bit = (result & (1 << shift)) >> shift;
        printf("%d", bit);
    }
    printf("\n");
}

int find_end(char* str, int start)
{
    int i = start;
    while (str[i] != ',' && str[i] != '\0')
        i++;
    return i;
}

int find_start(char*str, int last_end)
{
    int i = last_end;
    while (str[i] == ' ' || str[i] == '\t' || str[i] == ',')
        i++;
    return i;
}

int str_cmp(char* big_str, int start, int end, char* little_str)
{
    int i = 0; 
    for (i = 0; i < end - start; i++)
        if (big_str[start + i] != little_str[i])
            return -1;
    if (little_str[end - start] != '\0')
        return -1;

    return 0;
}

int is_digit(char c)
{
    return c <= '9' && c >= '0';
}

int get_num(char* str, int max_len, int* num_len)
{
    int ret = 0;
    int i = 0;
    *num_len = 0;
    
    for (i = 0; is_digit(str[i]) && i < max_len; i++)
    {
        ret *=10;
        ret += str[i] - '0';
        (*num_len)++;
    }
   
    return ret;
}

void get_range(char* str, int start, int end, int* ref_start, int* ref_end, int* ref_bit)
{
    if (str_cmp(str, start, end, "all") == 0)
    {
        *ref_start = 0;
        *ref_end = 31;
    }
    else if (str_cmp(str, start, end, "*") == 0)
    {
        *ref_start = 0;
        *ref_end = 31;
        
    }
    else if (str_cmp(str, start, end, "none") == 0)
    {
        *ref_start = 0;
        *ref_end = 31;
        *ref_bit = 1 - *ref_bit;
    }
    else if (str_cmp(str, start, end, "trace") == 0)
    {
        *ref_start = 31;
        *ref_end = 31;
    }
    else // range in numbers
    {
        if (str_cmp(str, start, end, "...") == 0) // ...
        {
            *ref_start = 0;
            *ref_end = 31; 
        }
        else
        {
            if (str_cmp(str, start, end, "..") == 0) // ..
            {
                *ref_start = 0;
                start += 2;

                if (is_digit(str[start]))  // .. num
                {
                    int num_len;
                    *ref_end = get_num(str + start, 2, &num_len);
                    start += num_len;
                }
                else 
                    printf("ERROR");
            }
            else if (is_digit(str[start])) // num
            {
                int num_len = 0;
                *ref_start = get_num(str + start, 2, &num_len);
                start += num_len; 

                if (str_cmp(str, start, start + 2, "..") == 0) // num .. 
                {
                    start += 2;
                    if (start == end) // num ..
                        *ref_end = 31;
                    else if (is_digit(str[start])) // num .. num
                    {
                        *ref_end = get_num(str + start, 2, &num_len);
                        start += num_len;
                    }
                    else
                        printf("ERROR");
                } 
                else if (start == end) // num
                   *ref_end = *ref_start; 
                else
                    printf("ERROR");

            }
            else 
                printf("ERROR");

            if (start != end)
                printf("ERROR");
        }
    }

    printf(" = (%d, %d)\n", *ref_start, *ref_end);
}

