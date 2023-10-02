#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    time_t t;
    struct tm *tmptr;
    char buf_1[100];
    char buf_2[100];
    char weekday[4];
    int day;

    time(&t);
    tmptr = localtime(&t);

    day = tmptr->tm_wday;

    switch(day){
        case 0:
            strcpy(weekday, "Sun");
            break;
        case 1:
            strcpy(weekday, "Mon");
            break;
        case 2:
            strcpy(weekday, "Tue");
            break;
        case 3:
            strcpy(weekday, "Wed");
            break;
        case 4:
            strcpy(weekday, "Thu");
            break;
        case 5:
            strcpy(weekday, "Fri");
            break;
        case 6:
            strcpy(weekday, "Sat");
            break;
    }

    //Oct 8
    strftime(buf_1, 100, "%b %d", tmptr);
    //2019 1:22 PM
    strftime(buf_2, 100, "%l:%M %p", tmptr);

    printf("%s(%s), %d%s\n", buf_1, weekday, 1900+tmptr->tm_year, buf_2);
   
    exit(EXIT_SUCCESS);

}