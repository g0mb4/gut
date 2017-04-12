/* gut.h - header file, that contains function prototype(s) and structure(s) for GUT
 * 2014, gmb */

#ifndef __GUT_H__
#define __GUT_H__

#define STAT_SIZE   10

typedef struct{
    char sym;
    char oldstat[STAT_SIZE];
    char newstat[STAT_SIZE];
    char write_val;
    char move;
    char halt;
}rule;

extern char **errors;

int process(rule *rules, int rule_ctr, char c, char *t, int *s, int *h);


#endif
