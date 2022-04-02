//assembler by Sebastien MacDougall-Landry
//License is available at
//https://github.com/EmperorPenguin18/assembler/blob/main/LICENSE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
size_t getline(char **, size_t *, FILE *);
#endif

int check_indexed(const char *);

void convert_register(char *);

void hex_to_binary(char *);

long dec_to_binary(int);

void negate(char *);

void convert_const(char *);

void convert_indexed(char *);

char* convert_line(const char *);

void removechar(char *, const int);

void remove_comments(char *);
