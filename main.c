/* main.c - main file for GUT
 * 2014, gmb */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gut.h"

char tape[256];
int head_pos = 0, status = 0;
int head_pos_tmp, status_tmp;
rule rules[128];
int rule_ctr = 0;

int handle_line(char *line);

char *help = "usage: gut -f <file> [-s]\n\n -f <file> - loads <file> and executes it\n -s - stepping mode(user interaction required)\n2014, gmb\n";

int main(int argc, char **argv)
{
    char *fname;
    FILE *fp;
    char line[128];
    int err = 0;
    unsigned int linenum = 0;
    char stepmode = 0;

    if(argc < 2){
        fprintf(stderr, "%s", help);
        return 1;
    }

    /* check switch options */
    int a;
    for(a = 0; a < argc; a++){
        if(argv[a][0] == '-'){
			switch(argv[a][1])
			{
				case 'f':
					fname = argv[++a];
					break;

				case 's':
					stepmode = 1;
					break;
			}
		}
    }

    /* load the file */
    if((fp = fopen(fname, "r")) == NULL){
        fprintf(stderr, "fopen() failed on %s", fname);
        return 2;
    }

    /* read file line by line */
    while(fgets(line, sizeof(line), fp) != NULL){
            linenum++;

            /* comment */
            if(line[0] == '#' || line[0] == ' ' || strlen(line) <= 1)
                continue;

            /* tape */
            if(line[0] == '<' && line[1] == 't'){
                while(1){
                    fgets(line, sizeof(line), fp);

                    if(line[0] == 't' && line[1] == '>')
                        break;

                    int n;

                    for(n = 0; line[n] != '\n'; n++)
                        tape[n] = line[n];

                    tape[n++] = '\0';
                }
            /* rules */
            } else if(line[0] == '<' && line[1] == 'r'){
                while(1){
                    fgets(line, sizeof(line), fp);

                    if(line[0] == 'r' && line[1] == '>')
                        break;

                    if((err = handle_line(line)) < 0){
                        fprintf(stderr, "%d: %s <- error\n", linenum, line);
                    }
                }
            }
    }

    fclose(fp);

    printf("--==[gut]==-- -[gmb]- -[2014]- -[v1.1]-\n\n");

    printf("t: %s\n\n", tape);

    /* init */
    head_pos = 0;
    status = 0;
    head_pos_tmp = 0;
    status_tmp = 0;
    int step = 0;
    int p = 0;

    /* execute the program */
    while(1){
        p = process(rules, rule_ctr, tape[head_pos], tape, &status, &head_pos);

        printf("%3d: %s | q: %d -> %d | h: %d -> %d", step, tape, status_tmp, status, head_pos_tmp, head_pos);

        if(p == -1){
            printf(" halt\n");
            break;
        }else{
            printf("\n");
        }

        if(p > 0){
            fprintf(stderr, "%3d: %s <-- error(%d)\n", step, tape, p);
            break;
        }

        step++;

        head_pos_tmp = head_pos;
        status_tmp = status;

        if(stepmode)
            getch();
    }

    return 0;
}

int handle_line(char *line){
    /* TODO: error checking */
    int ret = 0;

    rule r;

    /* init the strings */
    memset(r.oldstat, '\0', STAT_SIZE);
    memset(r.newstat, '\0', STAT_SIZE);

    /* read formatted rule */
    sscanf(line, "%c %s %s %c %c %c", &r.sym, r.oldstat, r.newstat, &r.write_val, &r.move, &r.halt);

    /* add rule to the ruleset */
    rules[rule_ctr++] = r;

    return ret;
}
