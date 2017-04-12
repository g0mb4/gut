/* gut.c - GUT function(s)
 * 2014, gmb */
#include "gut.h"

/* error messages */
char **errors = {"blank",
                 "unknown rule for newstat",
                 "unknown rule for move"
                 "no rule found"};

/* process a symbol based on the ruleset */
int process(rule *rules, int rule_ctr, char c, char *t, int *s, int *h){
    int i;
    char good = 1, good1, good2, good3;
    char found = 0;

    /* search for rule from the ruleset */
    for(i = 0; i < rule_ctr; i++){
        /* symbol found */
        if(rules[i].sym == c){
            char foo;
            int old_stat, new_stat;

            if((rules[i].oldstat[0] != '_'))
                sscanf(rules[i].oldstat, "%c%d", &foo, &old_stat);  /* read oldstat numerical value */

            /* is status rule correct ? */
            good1 = (rules[i].oldstat[0] == '_');   /* whatever */
            good2 = (rules[i].oldstat[0] == 'q' && *s == old_stat); /* equals */
            good3 = (rules[i].oldstat[0] == 'n' && *s != old_stat); /* not equals */

            good = good1 || good2 || good3;

            /* correct status rule */
            if(good){
                found = 1;

                /* new status value */
                /* plus */
                if(rules[i].newstat[0] == 'p')
                    if(rules[i].newstat[1] == 'p')
                        *s = *s + 1;
                    else {
                         sscanf(rules[i].newstat, "%c%d", &foo, &new_stat);
                        *s = *s + new_stat;
                    }
                /* minus */
                else if(rules[i].newstat[0] == 'm')
                    if(rules[i].newstat[1] == 'm')
                        *s = *s - 1;
                    else {
                        sscanf(rules[i].newstat, "%c%d", &foo, &new_stat);
                        *s = *s - new_stat;
                    }
                /* equal */
                else if(rules[i].newstat[0] == 'e') {
                    sscanf(rules[i].newstat, "%c%d", &foo, &new_stat);
                    *s = new_stat;
                /* whatever */
                } else if(rules[i].newstat[0] == '_'){}
                else
                    return 1;

                /* overwrite symbol ?*/
                if(rules[i].write_val != '_')
                    t[*h] = rules[i].write_val;

                /* move */
                if(rules[i].move == 'r')
                    *h = *h + 1;
                else if(rules[i].move == 'l')
                    *h = *h - 1;
                else if(rules[i].move == '_'){}
                else
                    return 2;

                /* halt */
                if(rules[i].halt == '1')
                    return -1;
            } else {
                /* symbol found, but status rule is not correct */
                continue;
            }
        }

        /* rule found, no need for searching */
        if(found)
            break;
    }

    if(found != 1){
        return 3;   /* no correct rule found for the symbol*/
    }else{
        return 0;
    }

}
