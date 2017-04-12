#include "asm.h"

int create_asm(char *fname, char *tape, rule *rules, int rule_ctr){

    FILE *fp;
    int i;

    if((fp = fopen(fname, "w")) == NULL)
        return -1;

    fprintf(fp, "# %s\n"
                "# generated assembly file from a GUT file\n\n", fname);

    /* variables */
    fprintf(fp, "\t.code32\n"
                "\t.section .data\n"
                "tape:\n"
                "\t.asciz \"%s\"\n"
                "output:\n"
                "\t.asciz \"%%s\\n\"\n"
                "errout:\n"
                "\t.asciz \"error (%%c)\\n\"\n"
                "state:\n"
                "\t.int 0\n\n", tape);

    /* initializations */
    fprintf(fp, "\t.section .text\n"
                "\t.globl _main\n"
                "_main:\n"
                "\tpushl $tape\n"
                "\tpushl $output\n"
                "\tcall _printf\n"
                "\tadd $8, %%esp\n"
                "\tmovl $0, %%edi\n"
                "\tmovb $0, %%al\n");

    fprintf(fp, "read:\n"
                "\tmovb tape(, %%edi, 1), %%al\n");

    char foo;
    int old_stat, new_stat;

    for(i = 0; i < rule_ctr; i++){

        /* no need 'next0' for the first symbol */
        if(i != 0)
            fprintf(fp, "next%d:\n", i);

        if(i == rule_ctr - 1){
            fprintf(fp, "\tmovb $%d, %%bl\t\t# %c\n"
                        "\tcmp %%al, %%bl\n" ,rules[i].sym, rules[i].sym);
        } else {
            fprintf(fp, "\tmovb $%d, %%bl\t\t# %c\n"
                        "\tcmp %%al, %%bl\n"
                        "\tjne next%d\n", rules[i].sym, rules[i].sym, (i + 1));
        }

        if(rules[i].oldstat[0] != '_'){
            sscanf(rules[i].oldstat, "%c%d", &foo, &old_stat);

            fprintf(fp, "\tmovb $%d, %%bl\n"
                        "\tmovb state, %%cl\n"
                        "\tcmp %%bl, %%cl\n", old_stat);

            if(i == rule_ctr - 1){
                if(foo == 'q')
                    fprintf(fp, "\tjne exiterr\t\t\t# q%d\n", old_stat);
                else if(foo == 'n')
                    fprintf(fp, "\tje exiterr\t\t\t# n%d\n", old_stat);
            } else {
                if(foo == 'q')
                    fprintf(fp, "\tjne next%d\t\t\t# q%d\n", (i + 1), old_stat);
                else if(foo == 'n')
                    fprintf(fp, "\tje next%d\t\t\t# n%d\n", (i + 1), old_stat);
            }
        }

        /* unconditional jump, because no matching rules were filtered out */
        fprintf(fp, "\tjmp S%d\n" , i);
    }

    for(i = 0; i < rule_ctr; i++){
        fprintf(fp, "S%d:\n", i);

        /* TODO : ex, px, mx */
        if(rules[i].newstat[0] != '_'){
            if(rules[i].newstat[0] == 'p'){
                if(rules[i].newstat[1] == 'p'){
                    fprintf(fp, "\tmovl $1, %%ecx\n"
                                "\taddl %%ecx, state\n");
                } else {
                    sscanf(rules[i].newstat, "%c%d", &foo, &new_stat);
                    fprintf(fp, "\tmovl $%d, %%ecx\n"
                                "\taddl %%ecx, state\n", new_stat);
                }
            } else if(rules[i].newstat[0] == 'm'){
                if(rules[i].newstat[1] == 'm'){
                    fprintf(fp, "\tmovl $1, %%ecx\n"
                                "\tsubl %%ecx, state\n");
                } else {
                    sscanf(rules[i].newstat, "%c%d", &foo, &new_stat);
                    fprintf(fp, "\tmovl $%d, %%ecx\n"
                                "\tsubl %%ecx, state\n", new_stat);
                }
            } else if(rules[i].newstat[0] == 'e'){
                    sscanf(rules[i].newstat, "%c%d", &foo, &new_stat);
                    fprintf(fp, "\tmovl $%d, %%ecx\n"
                                "\tmovl %%ecx, state\n", new_stat);
            }
        }

        if(rules[i].write_val != '_'){
            fprintf(fp, "\tmovb $%d, %%al\n"
                        "\tmovb %%al, tape(, %%edi, 1)\n", rules[i].write_val);
        }

        if(rules[i].move == 'r')
            fprintf(fp, "\taddl $1, %%edi\n");
        else if(rules[i].move == 'l')
            fprintf(fp, "\tsubl $1, %%edi\n");
        else if(rules[i].move == '_'){}

        if(rules[i].halt == '1'){
            fprintf(fp, "\tjmp exit\n");
        }

        fprintf(fp, "\tjmp read\n");

    }

    /* error */
    fprintf(fp, "exit:\n"
                "\tpushl $tape\n"
                "\tpushl $output\n"
                "\tcall _printf\n"
                "\tadd $8, %%esp\n"
                "\tpushl $0\n"
                "\tcall _exit\n");

    /* error on exit */
    fprintf(fp, "exiterr:\n"
                "\tpushl %%eax\n"
                "\tpushl $errout\n"
                "\tcall _printf\n"
                "\tadd $8, %%esp\n"
                "\tpushl %%eax\t\t# last readed character\n"
                "\tcall _exit\n");

    fclose(fp);

    return 0;
}
