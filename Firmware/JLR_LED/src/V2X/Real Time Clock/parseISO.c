
//TODO: Refactor to headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

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


int dateToEpoch(int year, int month, int day, int hour, int minute, int second, int offset);

void parseISO(time * buf_time, char *iso) {

  char * cptr = malloc(4*sizeof(char));

  memcpy(cptr, iso, 4);
  buf_time->year = atoi(cptr);

  cptr[2] = '\0';
  memcpy(cptr, iso+5, 2);
  buf_time->month = atoi(cptr);

  memcpy(cptr, iso+8, 2);
  buf_time->day = atoi(cptr);

  memcpy(cptr, iso+11, 2);
  buf_time->hour = atoi(cptr);

  memcpy(cptr, iso+14, 2);
  buf_time->minute = atoi(cptr);

  memcpy(cptr, iso+17, 2);
  buf_time->second = atoi(cptr);

  cptr[3] = '\0';
  memcpy(cptr, iso+19, 3);
  int offset_hr = atoi(cptr)*60*60;

  cptr[2] = '\0';
  memcpy(cptr, iso+23, 2);
  int offset_min = atoi(cptr)*60;

  int offset = 0;
  if (offset_hr >= 0)
    offset = offset_hr + offset_min;
  else
    offset = offset_hr - offset_min;

  buf_time->offset = offset;

  free(cptr);
}