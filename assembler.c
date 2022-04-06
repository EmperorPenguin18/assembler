//assembler by Sebastien MacDougall-Landry
//License is available at
//https://github.com/EmperorPenguin18/assembler/blob/main/LICENSE

#include "assembler.h"

#ifdef _WIN32
size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif

int check_indexed(const char s[]) {
	for (int i = 0; i < strlen(s); i++) {
		if (s[i] == '(') {
#ifdef DEBUG
			printf("check_indexed\ninput: %s\noutput: %d\n", s, i);
#endif
			return i;
		}
	}
#ifdef DEBUG
	printf("check_indexed\ninput: %s\noutput: %d\n", s, -1);
#endif
	return -1;
}

void convert_register(char *s) {
#ifdef DEBUG
	printf("convert_register\ninput: %s\n", s);
#endif
	char output[19] = "";
	int index = check_indexed(s);
	if (index++ != -1) {
		int i;
		for (i = index; i < strlen(s+index); i++) if (s[index+i] == ')') break;
		strncpy(output, s+index, i-1);
	}
	else {
		strcpy(output, s);
	}
	if (output[0] != 'R') {
		fprintf(stderr, "Unrecognized register: %s\n", s);
		exit(EXIT_FAILURE);
	}
	sprintf(s, "%04ld", dec_to_binary((int)strtol(output+1, NULL, 10)));
#ifdef DEBUG
	printf("output: %s\n", s);
#endif
}

void hex_to_binary(char *s) {
#ifdef DEBUG
	printf("hex_to_binary\ninput: %s\n", s);
#endif
	char *output = calloc(strlen(s), sizeof(char)*19);
	int i = 0;
	while (s[i])
	{
		switch (s[i])
		{
		case '0':
			strcat(output, "0000"); break;
		case '1':
			strcat(output, "0001"); break;
		case '2':
			strcat(output, "0010"); break;
		case '3':
			strcat(output, "0011"); break;
		case '4':
			strcat(output, "0100"); break;
		case '5':
			strcat(output, "0101"); break;
		case '6':
			strcat(output, "0110"); break;
		case '7':
			strcat(output, "0111"); break;
		case '8':
			strcat(output, "1000"); break;
		case '9':
			strcat(output, "1001"); break;
		case 'A':
			strcat(output, "1010"); break;
		case 'B':
			strcat(output, "1011"); break;
		case 'C':
			strcat(output, "1100"); break;
		case 'D':
			strcat(output, "1101"); break;
		case 'E':
			strcat(output, "1110"); break;
		case 'F':
			strcat(output, "1111"); break;
		case 'a':
			strcat(output, "1010"); break;
		case 'b':
			strcat(output, "1011"); break;
		case 'c':
			strcat(output, "1100"); break;
		case 'd':
			strcat(output, "1101"); break;
		case 'e':
			strcat(output, "1110"); break;
		case 'f':
			strcat(output, "1111"); break;
		default:
			fprintf(stderr, "Invalid hex digit %c\n", s[i]);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	sprintf(s, "%0*d%s", 19-strlen(output), 0, output);
	free(output);
#ifdef DEBUG
	printf("output: %s\n", s);
#endif
}

long dec_to_binary(int dno){
#ifdef DEBUG
	printf("dec_to_binary\ninput: %d\n", dno);
#endif
	long bno=0,rem,f=1;
	while(dno != 0){
		rem = dno % 2;
		bno = bno + rem * f;
		f = f * 10;
		dno = dno / 2;
	}
#ifdef DEBUG
	printf("output: %ld\n", bno);
#endif
	return bno;;
}

void negate(char *s) {
#ifdef DEBUG
	printf("negate\ninput: %s\n", s);
#endif
	int firstone = 0;
	for (int i = strlen(s)-1; i > -1; i--) {
		if (!firstone) {
			if (s[i] == '1') firstone = 1;
		}
		else {
			if (s[i] == '0') s[i] = '1';
			else s[i] = '0';
		}
	}
#ifdef DEBUG
	printf("output: %s\n", s);
#endif
}

void convert_const(char *s) {
#ifdef DEBUG
	printf("convert_const\ninput: %s\n", s);
#endif
	if (s[0] == '$') {
		removechar(s, 0);
		hex_to_binary(s);
	}
	else {
		if (s[0] == '-') {
			removechar(s, 0);
			sprintf(s, "%019ld", dec_to_binary((int)strtol(s, NULL, 10)));
			negate(s);
		}
		else {
			sprintf(s, "%019ld", dec_to_binary((int)strtol(s, NULL, 10)));
		}
	}
#ifdef DEBUG
	printf("output: %s\n", s);
#endif
}

void convert_indexed(char *s) {
#ifdef DEBUG
	printf("convert_indexed\ninput: %s\n", s);
#endif
	char output[19] = "";
	int i;
	for (i = 0; i < strlen(s); i++) if (s[i] == '(') break;
	strncpy(output, s, i-1);
	strcpy(s, output);
	convert_const(s);
#ifdef DEBUG
	printf("output: %s\n", s);
#endif
}

char* convert_line(const char line[]) {
	char binaryString[32] = "";
	char *instruction = calloc(4, sizeof(char)*4);
	char *ra = calloc(19, sizeof(char)*19);
	char *rb = calloc(19, sizeof(char)*19);
	char *rc = calloc(19, sizeof(char)*19);
	char *extra = calloc(19, sizeof(char)*19);
	sscanf(line, "%s%s%s%s", instruction, ra, rb, rc);
	if (!strcmp(instruction, "")) {
		strcpy(binaryString, "");
	}
	else if (!strcmp(instruction, "ld")) {
		strcat(binaryString, "00000");
		convert_register(ra);
		strcat(binaryString, ra);
		if (check_indexed(rb) != -1) {
			strcpy(extra, rb);
			convert_register(rb);
			strcat(binaryString, rb);
			convert_indexed(extra);
			strcat(binaryString, extra);
		}
		else {
			strcat(binaryString, "0000");
			convert_const(rb);
			strcat(binaryString, rb);
		}
	}
	else if (!strcmp(instruction, "ldi")) {
		strcat(binaryString, "00001");
		convert_register(ra);
		strcat(binaryString, ra);
		if (check_indexed(rb) != -1) {
			strcpy(extra, rb);
			convert_register(rb);
			strcat(binaryString, rb);
			convert_indexed(extra);
			strcat(binaryString, extra);
		}
		else {
			strcat(binaryString, "0000");
			convert_const(rb);
			strcat(binaryString, rb);
		}
	}
	else if (!strcmp(instruction, "st")) {
		strcat(binaryString, "00010");
		convert_register(rb);
		strcat(binaryString, rb);
		if (check_indexed(ra) != -1) {
			strcpy(extra, rb);
			convert_register(ra);
			strcat(binaryString, ra);
			convert_indexed(extra);
			strcat(binaryString, extra);
		}
		else {
			strcat(binaryString, "0000");
			convert_const(ra);
			strcat(binaryString, ra);
		}
	}
	else if (!strcmp(instruction, "add")) {
		strcat(binaryString, "00011");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "sub")) {
		strcat(binaryString, "00100");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "shr")) {
		strcat(binaryString, "00101");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "shl")) {
		strcat(binaryString, "00110");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "ror")) {
		strcat(binaryString, "00111");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "rol")) {
		strcat(binaryString, "01000");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "and")) {
		strcat(binaryString, "01001");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "or")) {
		strcat(binaryString, "01010");
		convert_register(ra);
		convert_register(rb);
		convert_register(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
		strcat(binaryString, "000000000000000");
	}
	else if (!strcmp(instruction, "addi")) {
		strcat(binaryString, "01011");
		convert_register(ra);
		convert_register(rb);
		convert_const(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
	}
	else if (!strcmp(instruction, "andi")) {
		strcat(binaryString, "01100");
		convert_register(ra);
		convert_register(rb);
		convert_const(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
	}
	else if (!strcmp(instruction, "ori")) {
		strcat(binaryString, "01101");
		convert_register(ra);
		convert_register(rb);
		convert_const(rc);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, rc);
	}
	else if (!strcmp(instruction, "mul")) {
		strcat(binaryString, "01110");
		convert_register(ra);
		convert_register(rb);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, "0000000000000000000");
	}
	else if (!strcmp(instruction, "div")) {
		strcat(binaryString, "01111");
		convert_register(ra);
		convert_register(rb);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, "0000000000000000000");
	}
	else if (!strcmp(instruction, "neg")) {
		strcat(binaryString, "10000");
		convert_register(ra);
		convert_register(rb);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, "0000000000000000000");
	}
	else if (!strcmp(instruction, "not")) {
		strcat(binaryString, "10001");
		convert_register(ra);
		convert_register(rb);
		strcat(binaryString, ra);
		strcat(binaryString, rb);
		strcat(binaryString, "0000000000000000000");
	}
	else if (!strcmp(instruction, "brzr")) {
		strcat(binaryString, "10010");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "0000");
		convert_const(rb);
		strcat(binaryString, rb);
	}
	else if (!strcmp(instruction, "brnz")) {
		strcat(binaryString, "10010");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "0001");
		convert_const(rb);
		strcat(binaryString, rb);
	}
	else if (!strcmp(instruction, "brpl")) {
		strcat(binaryString, "10010");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "0010");
		convert_const(rb);
		strcat(binaryString, rb);
	}
	else if (!strcmp(instruction, "brmi")) {
		strcat(binaryString, "10010");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "0011");
		convert_const(rb);
		strcat(binaryString, rb);
	}
	else if (!strcmp(instruction, "jr")) {
		strcat(binaryString, "10011");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "00000000000000000000000");
	}
	else if (!strcmp(instruction, "jal")) {
		strcat(binaryString, "10100");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "11110000000000000000000");
	}
	else if (!strcmp(instruction, "in")) {
		strcat(binaryString, "10101");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "00000000000000000000000");
	}
	else if (!strcmp(instruction, "out")) {
		strcat(binaryString, "10110");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "00000000000000000000000");
	}
	else if (!strcmp(instruction, "mfhi")) {
		strcat(binaryString, "10111");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "00000000000000000000000");
	}
	else if (!strcmp(instruction, "mflo")) {
		strcat(binaryString, "11000");
		convert_register(ra);
		strcat(binaryString, ra);
		strcat(binaryString, "00000000000000000000000");
	}
	else if (!strcmp(instruction, "nop")) {
		strcat(binaryString, "11001");
		strcat(binaryString, "000000000000000000000000000");
	}
	else if (!strcmp(instruction, "halt")) {
		strcat(binaryString, "11010");
		strcat(binaryString, "000000000000000000000000000");
	}
	else {
		fprintf(stderr, "Unsupported instruction detected.\n");
		exit(EXIT_FAILURE);
	}
	char *hexString = calloc(8, sizeof(char)*8);
	sprintf(hexString, "%08x", (int)strtol(binaryString, NULL, 2));
	free(instruction);
	free(ra);
	free(rb);
	free(rc);
	free(extra);
#ifdef DEBUG
	printf("convert_line\ninput: %s\noutput: %s\n", line, hexString);
#endif
	return hexString;
}

void removechar(char *s, const int n) {
#ifdef DEBUG
	printf("removechar\ninputs: %s, %d\n", s, n);
#endif
	for (int i = n; i < strlen(s); i++)
		s[i] = s[i+1];
#ifdef DEBUG
	printf("output: %s\n", s);
#endif
}

void remove_comments(char *line) {
#ifdef DEBUG
	printf("remove_comments\ninput: %s\n", line);
#endif
	char *output = calloc(strlen(line), sizeof(char)*strlen(line));
	int i;
	for (i = 0; i < strlen(line); i++) {
		if (line[i] == ';') break;
		if (line[i] == ',') removechar(line, i--);
	}
	strncpy(output, line, i);
	strcpy(line, output);
	free(output);
#ifdef DEBUG
	printf("output: %s\n", line);
#endif
}

int main(int argc, char ** argv) {
	if (argc < 3) {
		fprintf(stderr, "Not enough arguments.\n");
		exit(EXIT_FAILURE);
	}

	FILE * fp_r;
	FILE * fp_w;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp_r = fopen(argv[1], "r");
	if (fp_r == NULL) {
		fprintf(stderr, "Failed to open file to read.\n");
		exit(EXIT_FAILURE);
	}
	fp_w = fopen(argv[2], "w");
	if (fp_w == NULL) {
		fprintf(stderr, "Failed to open file to write.\n");
		exit(EXIT_FAILURE);
	}

	fprintf(fp_w, "v3.0 hex words addressed");
	int i = 0;
	while ((read = getline(&line, &len, fp_r)) != -1) {
		if (i % 8 == 0) fprintf(fp_w, "\n%03x: ", i);
		remove_comments(line);
		char * output = convert_line(line);
		fprintf(fp_w, "%s ", output);
		free(output);
		i++;
	}
	fprintf(fp_w, "\n");

	fclose(fp_r);
	fclose(fp_w);
	if (line) free(line);
	exit(EXIT_SUCCESS);
}
