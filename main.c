#include <stdio.h>
#include <stdlib.h>

typedef struct{
    char sym;
    char oldstat[2];
    char newstat[2];
    char write_val;
    char move;
    char halt;
}rule;

char tape[256];
int head_pos = 0, status = 0;
rule rules[128];
int rule_ctr = 0;

int handel_line(char *line);
int process(char c, char *t, int *s, int *h);

char *help = "usage: gut -f <file> [-s]\n\n -f <file> - loads <file> and executes it\n -s - stepping mode\n2014, gmb\n";

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

                    if((err = handel_line(line)) < 0){
                        fprintf(stderr, "%d: %s <- error\n", linenum, line);
                    }
                }
            }
    }

    fclose(fp);

    printf("--==[gut]==-- -[gmb]- -[2014]- -[v1.0]-\n\n");

    printf("t: %s\n\n", tape);

    /* init */
    head_pos = 0;
    status = 0;
    int step = 0;
    int p = 0;

    /* execute the program */
    while(1){
        p = process(tape[head_pos], tape, &status, &head_pos);

        printf("%3d: %s | s: %d | h: %d", step, tape, status, head_pos);

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
        if(stepmode)
            getch();
    }

    return 0;
}

int handel_line(char *line){
    int ret = 0;

    rule r;

    sscanf(line, "%c %s %s %c %c %c", &r.sym, r.oldstat, r.newstat, &r.write_val, &r.move, &r.halt);
    rules[rule_ctr++] = r;

    return ret;
}

int process(char c, char *t, int *s, int *h){
    int i;
    char good = 1, good1, good2, good3;
    char found = 0;

    for(i = 0; i < rule_ctr; i++){
        /* symbol found */
        if(rules[i].sym == c){
            good1 = (rules[i].oldstat[0] == '_');
            good2 = (rules[i].oldstat[0] == 'e' && *s == (int)(rules[i].oldstat[1] - '0'));
            good3 = (rules[i].oldstat[0] == 'n' && *s != (int)(rules[i].oldstat[1] - '0'));

            good = good1 || good2 || good3;

            if(good){
                found = 1;

                if(rules[i].newstat[0] == 'p')
                    if(rules[i].newstat[1] == 'p')
                        *s = *s + 1;
                    else
                        *s = *s + (int)(rules[i].newstat[1] - '0');

                else if(rules[i].newstat[0] == 'm')
                    if(rules[i].newstat[1] == 'm')
                        *s = *s - 1;
                    else
                        *s = *s - (int)(rules[i].newstat[1] - '0');
                else if(rules[i].newstat[0] == '_'){}
                else
                    return 1;


                if(rules[i].write_val != '_')
                    t[*h] = rules[i].write_val;

                if(rules[i].move == 'r')
                    *h = *h + 1;
                else if(rules[i].move == 'l')
                    *h = *h - 1;
                else if(rules[i].move == '_'){}
                else
                    return 2;

                if(rules[i].halt == '1')
                    return -1;
            } else {
                continue;
            }
        }

        if(found)
            break;
    }

    if(found != 1){
        return 3;
    }else{
        return 0;
    }

}
