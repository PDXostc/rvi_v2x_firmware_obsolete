/*
 * epoch.h
 *
 * Created: 11/24/2015 10:12:28 PM
 *  Author: Kevin
 */ 


#ifndef EPOCH_H_
#define EPOCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct time_t {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int offset;
};

typedef struct time_t time;

int dateToEpoch(int, int, int, int, int, int, int);
void parseISO(time *, char *);

#endif /* EPOCH_H_ */