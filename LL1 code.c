// M Hamid Jamil
// SP19-BCS-098
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 128
int table[100][SIZE];
char terminal[SIZE];
char nonTerminal[26];
struct product {
    char str[100];
    int len;
} prod_[20];
int prd_no;
char first_set[26][SIZE];
char follow_set[26][SIZE];
char first_rhs[100][SIZE];

int is_non_terminal(char c) { return c >= 'A' && c <= 'Z'; }

void readFromFile() {
    FILE *fp;
    fp = fopen("input.txt", "r");
    char buffer[255];
    int i;
    int j;
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
        j = 0;
        nonTerminal[buffer[0] - 'A'] = 1;
        for (i = 0; i < strlen(buffer) - 1; ++i) {
            if (buffer[i] == '|') {
                ++prd_no;
                prod_[prd_no - 1].str[j] = '\0';
                prod_[prd_no - 1].len = j;
                prod_[prd_no].str[0] = prod_[prd_no - 1].str[0];
                prod_[prd_no].str[1] = prod_[prd_no - 1].str[1];
                prod_[prd_no].str[2] = prod_[prd_no - 1].str[2];
                j = 3;
            } else {
                prod_[prd_no].str[j] = buffer[i];
                ++j;
                if (!is_non_terminal(buffer[i]) && buffer[i] != '-' &&
                    buffer[i] != '>') {
                    terminal[buffer[i]] = 1;
                }
            }
        }
        prod_[prd_no].len = j;
        ++prd_no;
    }
}

void first_to_follow(char A, char B) {
    int i;
    for (i = 0; i < SIZE; ++i) {
        if (i != '^')
            follow_set[B - 'A'][i] = follow_set[B - 'A'][i] || first_set[A - 'A'][i];
    }
}

void follow_to_follow(char A, char B) {
    int i;
    for (i = 0; i < SIZE; ++i) {
        if (i != '^')
            follow_set[B - 'A'][i] = follow_set[B - 'A'][i] || follow_set[A - 'A'][i];
    }
}

void FOLLOW() {
    int t = 0;
    int i, j, k, x;
    while (t++ < prd_no) {
        for (k = 0; k < 26; ++k) {
            if (!nonTerminal[k])
                continue;
            char nt = k + 'A';
            for (i = 0; i < prd_no; ++i) {
                for (j = 3; j < prod_[i].len; ++j) {
                    if (nt == prod_[i].str[j]) {
                        for (x = j + 1; x < prod_[i].len; ++x) {
                            char sc = prod_[i].str[x];
                            if (is_non_terminal(sc)) {
                                first_to_follow(sc, nt);
                                if (first_set[sc - 'A']['^'])
                                    continue;
                            } else {
                                follow_set[nt - 'A'][sc] = 1;
                            }
                            break;
                        }
                        if (x == prod_[i].len)
                            follow_to_follow(prod_[i].str[0], nt);
                    }
                }
            }
        }
    }
}

void add_FIRST_A_to_FIRST_B(char A, char B) {
    int i;
    for (i = 0; i < SIZE; ++i) {
        if (i != '^') {
            first_set[B - 'A'][i] = first_set[A - 'A'][i] || first_set[B - 'A'][i];
        }
    }
}

void FIRST() {
    int i, j;
    int t = 0;
    while (t < prd_no) {
        for (i = 0; i < prd_no; ++i) {
            for (j = 3; j < prod_[i].len; ++j) {
                char sc = prod_[i].str[j];
                if (is_non_terminal(sc)) {
                    add_FIRST_A_to_FIRST_B(sc, prod_[i].str[0]);
                    if (first_set[sc - 'A']['^'])
                        continue;
                } else {
                    first_set[prod_[i].str[0] - 'A'][sc] = 1;
                }
                break;
            }
            if (j == prod_[i].len)
                first_set[prod_[i].str[0] - 'A']['^'] = 1;
        }
        ++t;
    }
}

void add_FIRST_A_to_FIRST_RHS__B(char A, int B) {
    int i;
    for (i = 0; i < SIZE; ++i) {
        if (i != '^')
            first_rhs[B][i] = first_set[A - 'A'][i] || first_rhs[B][i];
    }
}

void FIRST_RHS() {
    int i, j;
    int t = 0;
    while (t < prd_no) {
        for (i = 0; i < prd_no; ++i) {
            for (j = 3; j < prod_[i].len; ++j) {
                char sc = prod_[i].str[j];
                if (is_non_terminal(sc)) {
                    add_FIRST_A_to_FIRST_RHS__B(sc, i);
                    if (first_set[sc - 'A']['^'])
                        continue;
                } else {
                    first_rhs[i][sc] = 1;
                }
                break;
            }
            if (j == prod_[i].len)
                first_rhs[i]['^'] = 1;
        }
        ++t;
    }
}

int main() {
    readFromFile();
    follow_set[prod_[0].str[0] - 'A']['$'] = 1;
    FIRST();
    FOLLOW();
    FIRST_RHS();
    int i, j, k;

    printf("\n");
    for (i = 0; i < prd_no; ++i) {
        if (i == 0 || (prod_[i - 1].str[0] != prod_[i].str[0])) {
            char c = prod_[i].str[0];
            printf("FIRST OF %c: ", c);
            for (j = 0; j < SIZE; ++j) {
                if (first_set[c - 'A'][j]) {
                    printf("%c ", j);
                }
            }
            printf("\n");
        }
    }

    printf("\n");
    for (i = 0; i < prd_no; ++i) {
        if (i == 0 || (prod_[i - 1].str[0] != prod_[i].str[0])) {
            char c = prod_[i].str[0];
            printf("FOLLOW OF %c: ", c);
            for (j = 0; j < SIZE; ++j) {
                if (follow_set[c - 'A'][j]) {
                    printf("%c ", j);
                }
            }
            printf("\n");
        }
    }
    printf("\n");
    for (i = 0; i < prd_no; ++i) {
        printf("FIRST OF %s: ", prod_[i].str);
        for (j = 0; j < SIZE; ++j) {
            if (first_rhs[i][j]) {
                printf("%c ", j);
            }
        }
        printf("\n");
    }
    terminal['$'] = 1;
    terminal['^'] = 0;

    // printing parse table
    printf("\n");
    printf("\n\t                 LL(1) Parsing Table                 \n");
    printf("\t________________________________________________________\n");
    printf("%-10s", "");
    for (i = 0; i < SIZE; ++i) {
        if (terminal[i])
            printf("%-10c", i);
    }
    printf("\n");
    int p = 0;
    for (i = 0; i < prd_no; ++i) {
        if (i != 0 && (prod_[i].str[0] != prod_[i - 1].str[0]))
            p = p + 1;
        for (j = 0; j < SIZE; ++j) {
            if (first_rhs[i][j] && j != '^') {
                table[p][j] = i + 1;
            } else if (first_rhs[i]['^']) {
                for (k = 0; k < SIZE; ++k) {
                    if (follow_set[prod_[i].str[0] - 'A'][k]) {
                        table[p][k] = i + 1;
                    }
                }
            }
        }
    }
    k = 0;
    for (i = 0; i < prd_no; ++i) {
        if (i == 0 || (prod_[i - 1].str[0] != prod_[i].str[0])) {
            printf("%-10c", prod_[i].str[0]);
            for (j = 0; j < SIZE; ++j) {
                if (table[k][j]) {
                    printf("%-10s", prod_[table[k][j] - 1].str);
                } else if (terminal[j]) {
                    printf("%-10s", "");
                }
            }
            ++k;
            printf("\n");
        }
    }
}