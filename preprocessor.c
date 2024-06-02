#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct definitions
struct ArrayTable {
    char name[10];
    int dim;  // 1 for 1D, 2 for 2D
    char size1[10];  // size of dimension 1
    char size2[10];  // size of dimension 2
};
struct ArrayTable AT[20];  // max. 20 arrays
int arrayNumber = 0;

struct ParseTable {
    char oper[10];  // @read, @copy, etc.
    char lhs[10];   // left-hand side array
    char rhs1[10];  // first right-hand side array
    char rhs2[10];  // second right-hand side array
};
struct ParseTable PT;

void parseLine(char *line);
void declareArr(FILE *output);
void readFromFile(FILE *output);
void mmult(FILE *output);
void printArr(FILE *output);
void initialize(FILE *output);
void dotp(FILE *output);
void copy(FILE *output);
void addition(FILE *output);
void arrSum(FILE *output);
void arrAvg(FILE *output);
void addArrayToTable(int wordCount, char buffer[]);
void addTokenToTable(int wordCount, char buffer[]);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    FILE *expanded = fopen("expanded.c", "w");

    if (file == NULL) {
        printf("Error opening the file!\n");
        return 1;
    }

    if (expanded == NULL) {
        printf("Error opening the output file!\n");
        fclose(file);
        return 1;
    }

    char buffer[100];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        int i = 0;
        while (buffer[i] == '\t' || buffer[i] == ' ') {
            i++;
        }

        if (buffer[i] == '@') {
            parseLine(&buffer[i]);
            if (strcmp(PT.oper, "@int") == 0) {
                declareArr(expanded);
            } else if (strcmp(PT.oper, "@read") == 0) {
                readFromFile(expanded);
            } else if (strcmp(PT.oper, "@mmult") == 0) {
                mmult(expanded);
            } else if (strcmp(PT.oper, "@print") == 0) {
                printArr(expanded);
            } else if (strcmp(PT.oper, "@init") == 0) {
                initialize(expanded);
            } else if (strcmp(PT.oper, "@dotp") == 0) {
                dotp(expanded);
            } else if (strcmp(PT.oper, "@copy") == 0) {
                copy(expanded);
            } else if (strcmp(PT.oper, "@add") == 0) {
                addition(expanded);
            } else if (strcmp(PT.oper, "@sum") == 0) {
                arrSum(expanded);
            } else if (strcmp(PT.oper, "@aver") == 0) {
                arrAvg(expanded);
            }
        } else {

            fputs(buffer, expanded);
        }
    }
    fclose(file);
    fclose(expanded);
    
    return 0;
}

void parseLine(char *line) {
    char buffer[10];
    int wordCount = 0;
    int j = 0;
    for (int i = 0; line[i] != '\n'; i++) {
        if (line[i] == ' ' || line[i] == '(' || line[i] == ')' || line[i] == '<' || line[i] == '>' ||line[i] == '=' || line[i] == '.' || line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == ';' || line[i] == ',') {
            buffer[j] = '\0';
            if (!strcmp(buffer, ""))
                continue;

            if (wordCount == 0) {
                strcpy(PT.oper, buffer);
            } else {
                if (!strcmp(PT.oper, "@int")) {
                    addArrayToTable(wordCount, buffer);
                }

                else if (!strcmp(PT.oper, "@read") || !strcmp(PT.oper, "@init") || !strcmp(PT.oper, "@print") || !strcmp(PT.oper, "@sum") || !strcmp(PT.oper, "@aver") || !strcmp(PT.oper, "@copy")) {
                    addTokenToTable(wordCount, buffer);
                }

                else if (!strcmp(PT.oper, "@dotp") || !strcmp(PT.oper, "@add") || !strcmp(PT.oper, "@mmult")) {
                    addTokenToTable(wordCount, buffer);

                }
            }
            wordCount++;
            memset(buffer, 0, sizeof buffer);
            j = 0;
        } else {
            buffer[j] = line[i];
            j++;
        }
    }

    if (!strcmp(PT.oper, "@int"))
        arrayNumber++;
}

void addArrayToTable(int wordCount, char buffer[]) {
    if(wordCount == 1){
        strcpy(AT[arrayNumber].name, buffer);
        strcpy(PT.lhs, buffer);
    }
    else if(wordCount == 2){
        strcpy(AT[arrayNumber].size1, buffer);
        AT[arrayNumber].dim++;
    }
    else if(wordCount == 3){
        strcpy(AT[arrayNumber].size2, buffer);
        AT[arrayNumber].dim++;
    }
}

void addTokenToTable(int wordCount, char buffer[]){
    if (wordCount == 1) {
        strcpy(PT.lhs, buffer);
    }
    else if (wordCount == 2){
        strcpy(PT.rhs1, buffer);
    }
    else if(wordCount == 3){
        strcpy(PT.rhs2, buffer);
    }
}

void readFromFile(FILE *output) {
    fprintf(output, "\tFILE *file;\n");
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.lhs) == 0) {
            fprintf(output, "\tfile_read = fopen(\"%s\", \"r\");\n", PT.rhs1);
            fprintf(output, "\tif(file_read == NULL){\n"
                      "\t\tprintf(\"File not found!\");\n"
                      "\t\treturn 1;\n"
                      "\t}\n");
            if (AT[i].dim == 1) {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\t\tfscanf(file_read, \"%%d\", &%s[i]);\n", AT[i].size1, PT.lhs);
            } else {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\t\tfor (int j = 0; j < %s; ++j) \n\t\t\tfscanf(file_read, \"%%d\", &%s[i][j]);\n", AT[i].size1, AT[i].size2, PT.lhs);
            }
            break;
        }
    }
    fprintf(output, "\tfclose(file_read);\n");
}

void mmult(FILE *output) {
    struct ArrayTable elm1, elm2;
    int found1 = 0, found2 = 0;

    for (int i = 0; i < arrayNumber; i++) {
        if (!strcmp(PT.rhs1, AT[i].name)) {
            elm1 = AT[i];
            found1 = 1;
        }
        if (!strcmp(PT.rhs2, AT[i].name)) {
            elm2 = AT[i];
            found2 = 1;
        }
    }

    if (!found1 || !found2) {
        fprintf(stderr, "One or both arrays not found!\n");
        return;
    }

    strcpy(AT[arrayNumber].name, PT.lhs);
    strcpy(AT[arrayNumber].size1, elm1.size1);
    strcpy(AT[arrayNumber].size2, elm2.size2);
    AT[arrayNumber].dim = 2;
    arrayNumber++;

    for (int i = 0; i < arrayNumber; ++i) {
        for (int j = 0; j < arrayNumber; ++j) {
            if (strcmp(AT[i].name, PT.rhs1) == 0 && strcmp(AT[j].name, PT.rhs2) == 0 && AT[i].dim == 2 && AT[j].dim == 2) {
                fprintf(output, "\tfor (int x = 0; x < %s; ++x) {\n", AT[i].size1);
                fprintf(output, "\t    for (int y = 0; y < %s; ++y) {\n", AT[j].size2);
                fprintf(output, "\t        %s[x][y] = 0;\n", PT.lhs);
                fprintf(output, "\t        for (int k = 0; k < %s; ++k) {\n", AT[i].size2);
                fprintf(output, "\t            %s[x][y] += %s[x][k] * %s[k][y];\n", PT.lhs, PT.rhs1, PT.rhs2);
                fprintf(output, "\t        }\n");
                fprintf(output, "\t    }\n");
                fprintf(output, "\t}\n");
                return;
            }
        }    
    }
}

void initialize(FILE *output) {
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.lhs) == 0) {
            if (AT[i].dim == 1) {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n \t\t %s[i] = %d;\n", AT[i].size1, PT.lhs, atoi(PT.rhs1));
            } else {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\t\tfor (int j = 0; j < %s; ++j) \n\t\t\t%s[i][j] = %d;\n", AT[i].size1, AT[i].size2, PT.lhs, atoi(PT.rhs1));
            }
        }
    }
}

void printArr(FILE *output) {
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.lhs) == 0) {
            if (AT[i].dim == 1) {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\t\t printf(\"%%d \", %s[i]);\n", AT[i].size1, PT.lhs);
            } else {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) {\n\t\t for (int j = 0; j < %s; ++j) \n\t\tprintf(\"%%d \", %s[i][j]);\n\t}\n", AT[i].size1, AT[i].size2, PT.lhs);
            }
            fprintf(output, "\tprintf(\"\\n\");\n");
            break;
        }
    }
}

void arrSum(FILE *output) {
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.lhs) == 0) {
            fprintf(output, "\tint P_sum = 0;\n");
            if (AT[i].dim == 1) {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\tP_sum += %s[i];\n", AT[i].size1, PT.lhs);
            } else {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\t\tfor (int j = 0; j < %s; ++j) \n\t\t\tP_sum += %s[i][j];\n", AT[i].size1, AT[i].size2, PT.lhs);
            }
            fprintf(output, "\tprintf(\"Sum: %%d\\n\", P_sum);\n");
            break;
        }
    }
}

void arrAvg(FILE *output) {
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.lhs) == 0) {
            fprintf(output, "\tint P_aver = 0;\n");
            fprintf(output, "\tint P_sum = 0;\n");
            if (AT[i].dim == 1) {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\t\tsum += %s[i];\n", AT[i].size1, PT.lhs);
                fprintf(output, "\tprintf(\"Average: %%f\\n\", sum / (float)%s);\n", AT[i].size1);
            } else {
                fprintf(output, "\tfor (int i = 0; i < %s; ++i) \n\t\tfor (int j = 0; j < %s; ++j) \n\t\t\tsum += %s[i][j];\n", AT[i].size1, AT[i].size2, PT.lhs);
                fprintf(output, "\tprintf(\"Average: %%f\\n\", sum / (float)(%s * %s));\n", AT[i].size1, AT[i].size2);
            }
            break;
        }
    }
}

void copy(FILE *output) {
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.lhs) == 0) {
            if (AT[i].dim == 1) {
                fprintf(output, "for (int i = 0; i < %s; ++i) %s[i] = %s[i];\n", AT[i].size1, PT.lhs, PT.rhs1);
            } else {
                fprintf(output, "for (int i = 0; i < %s; ++i) for (int j = 0; j < %s; ++j) %s[i][j] = %s[i][j];\n", AT[i].size1, AT[i].size2, PT.lhs, PT.rhs1);
            }
            break;
        }
    }
}

void addition(FILE *output) {
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.rhs1) == 0) {
            if (AT[i].dim == 1) {
                fprintf(output, "for (int i = 0; i < %s; ++i) %s[i] = %s[i] + %s[i];\n", AT[i].size1, PT.lhs, PT.rhs1, PT.rhs2);
            } else {
                fprintf(output, "for (int i = 0; i < %s; ++i) for (int j = 0; j < %s; ++j) %s[i][j] = %s[i][j] + %s[i][j];\n", AT[i].size1, AT[i].size2, PT.lhs, PT.rhs1, PT.rhs2);
            }
            break;
        }
    }
}

void declareArr(FILE *output) {

    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.lhs) == 0) {
            if (AT[i].dim == 1) {
                fprintf(output, "\tint %s[%s];\n", PT.lhs, AT[i].size1);
            } else {
                fprintf(output, "\tint %s[%s][%s];\n", PT.lhs, AT[i].size1, AT[i].size2);
            }
            break;
        }
    }
}

void dotp(FILE *output) {
    for (int i = 0; i < 20; ++i) {
        if (strcmp(AT[i].name, PT.rhs1) == 0 && AT[i].dim == 1) {
            fprintf(output, "\tint %s = 0;\n", PT.lhs);
            fprintf(output, "\tfor (int i = 0; i < %s; ++i)\n\t\t%s += %s[i] * %s[i];\n", AT[i].size1, PT.lhs, PT.rhs1, PT.rhs2);
            break;
        }
    }
}