#ifndef DAVELIB_TOOLS_H
#define DAVELIB_TOOLS_H

#define SPEEDTESTINIT() clock_t begin; clock_t end; double timespent;

#define SPEEDTESTBEGIN() begin = clock();
#define SPEEDTESTEND() end = clock();
#define CALCSPEED() timespent = (double)(end - begin);

#define PI 3.14159265358979323846


#endif
