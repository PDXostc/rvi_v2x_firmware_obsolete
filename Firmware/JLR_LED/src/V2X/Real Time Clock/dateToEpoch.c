/*#include <stdio.h>
#include <time.h>*/

int months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int dateToEpoch(int year, int month, int day, int hour, int minute, int second, int offset) {
  
  unsigned int leap_years = 0;
  int elapsed_years = year - 1970;
  int scalar = 365 * 24 * 60 * 60;
  int elapsed_sec = elapsed_years * scalar;
  //printf("\nElapsed seconds in the %d year(s) since 1970: %d\n", elapsed_years, elapsed_sec);
  
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
  
  //printf("Elapsed seconds plus the %d day(s) for the %d leap years past: %d\n", leap_years, leap_years, elapsed_sec);
  int elapsed_days = 0;
  
 
   for (i=0; i<month-1; i++) {
       elapsed_days += months[i];
   }
   elapsed_days += day - 1;
  
  
  
  elapsed_sec += elapsed_days*24*60*60;
  //printf("Elapsed seconds for the %d days since January 1st, %d: %d\n", elapsed_days, year, elapsed_sec);
  elapsed_sec += hour*60*60 + minute*60;

  elapsed_sec += second;
  elapsed_sec += offset;

  return (elapsed_sec);
}