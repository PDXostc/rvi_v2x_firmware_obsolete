#include "epoch.h"

int months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int dateToEpoch(int year, int month, int day, int hour, int minute, int second, int offset) {
  
  unsigned int leap_years = 0;
  int elapsed_years = year - 1970;
  int scalar = 365 * 24;
  scalar *= (60 * 60); //workaround overflow warning
  int elapsed_sec = elapsed_years * scalar;

  
  int i;
  
  for(i=1972; i < year; i += 4) {
      if(i%4 == 0 && (i%100 != 0 || i%400 == 0)) {
          leap_years++;   //add an extra day for each leap year
      }
  }
  
  if ( month>2 && year%4 == 0 && (year%100 != 0 || year%400 == 0)) {
      elapsed_sec += (leap_years + 1 )*24*60*60;
  }
  else {
      elapsed_sec += (leap_years)*24*60*60;
  }
  
  int elapsed_days = 0;
 
   for (i=0; i<month-1; i++) {
       elapsed_days += months[i];
   }
   elapsed_days += day - 1;
  
  elapsed_sec += elapsed_days*24*60*60;
  elapsed_sec += hour*60*60 + minute*60;
  elapsed_sec += second;
  elapsed_sec += offset;

  return (elapsed_sec);
}

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