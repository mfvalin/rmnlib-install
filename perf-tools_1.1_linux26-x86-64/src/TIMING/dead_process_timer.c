#include <unistd.h>

static int maximum_delay=0;

/* 
  the interval between calls to routine i_am_alive must be no less than
  maximum_delay seconds, otherwise the process will be aborted with exit(1)
  (this routine is callable from C or FORTRAN)
  in C:       i_am_alive()
  in FORTRAN: call i_am_alive()
*/

#pragma weak i_am_alive_=i_am_alive
#pragma weak i_am_alive__=i_am_alive

#pragma weak start_dead_process_timer_=start_dead_process_timer
#pragma weak start_dead_process_timer__=start_dead_process_timer

#if ! defined(USE_LEGACY_CODE)

void i_am_alive()
{
  alarm(maximum_delay);
}

/* more modern version using the alarm signael */
void start_dead_process_timer(unsigned int *timeout)
{
  maximum_delay = *timeout;
  i_am_alive();
}

#else

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

static struct timeval timeref;

/* 
  the interval between calls to routine i_am_alive must be no less than
  maximum_delay seconds, otherwise the process will be aborted with exit(1)
  (this routine is callable from C or FORTRAN)
  in C:       i_am_alive()
  in FORTRAN: call i_am_alive()
*/

void i_am_alive()
{
gettimeofday(&timeref,NULL);
}


/*
  dead process timer loop (started inside a thread)
  the routine gets the current time every second
  if more than maximum_delay seconds have elapsed since the
  last call to i_am_alive, the process is killed with
  exit(1)
*/
static void *dead_process_timer(void *not_used)
{
  struct timeval timenow;
  useconds_t sleep_interval=1000000;  /* 1 second */
  int inactivity;

  while(1){
    usleep(sleep_interval);   /* microsleep (thread sleep), not process sleep */
    gettimeofday(&timenow,NULL);
    inactivity=timenow.tv_sec-timeref.tv_sec;
#if defined(TEST) || defined(TEST2)
    fprintf(stderr,"process has been inactive for %d seconds\n",inactivity);
#endif
    if(timenow.tv_sec-timeref.tv_sec>maximum_delay){
      fprintf(stderr,"FATAL ERROR: process not responding for more than %d seconds, ABORTING\n",maximum_delay);
      exit(1); /* process assumed stuck, abort */
    }
  }
  return(NULL);
}

static pthread_attr_t thread_attributes;
static pthread_t thread_id;

/*
  start the watchdog routine dead_process_timer as a thread
  in C:       start_dead_process_timer(&timeout_in_seconds)
  in FORTRAN: call start_dead_process_timer(timeout_in_seconds)
  timeout_in_seconds is of type INTEGER (FORTRAN) or int (C)

  subsequent calls to start_dead_process_timer will only update the tiemout value
*/

void start_dead_process_timer(int *timeout)
{
  if(maximum_delay!=0){  /* if thread already started, only update timeout value */
    maximum_delay=*timeout;
    return;
  }
  maximum_delay=*timeout;
  i_am_alive();
  pthread_attr_init(&thread_attributes);
  pthread_create(&thread_id,&thread_attributes,dead_process_timer,NULL);
}
#endif


#ifdef TEST
#include <stdio.h>
/* idiot TEST program */
void main()
{
int timeout=15; /* set timeout to 15 seconds */
useconds_t j=5000000;

start_dead_process_timer(&timeout);
while(1){
fprintf(stderr,"sleeping %d seconds \n",j/1000000);
i_am_alive();
usleep(j);
j=j+3000000;
}
}
#endif
