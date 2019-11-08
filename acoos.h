/*
 * Library   COOS - COoperative Operating System
 * Author    A.Kouznetsov
 * Rev       1.6 dated 13/10/2019
 * Target    Arduino
Redistribution and use in source and binary forms, with or without modification, 
are permitted.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#ifndef __COOS_H
#define __COOS_H

//##############################################################################
// Inc
//##############################################################################

#include <Arduino.h>
#include <setjmp.h>

//##############################################################################
// Typedef
//##############################################################################

#ifndef __UCHAR_DEFINED__
  #define __UCHAR_DEFINED__
  typedef unsigned char uchar;
  typedef signed   char schar;
  typedef unsigned int  uint;
  typedef unsigned long ulong;
  typedef signed   long slong;
#endif

//##############################################################################
// Def                                              
//##############################################################################

#define COOS_STOP            -2    // set this value to stop task
#define COOS_DELAY(__delay)  if (!setjmp(coos.task_context[coos.task_no])) {longjmp(coos.main_context, __delay+1);} else{}

#define COOS_REV_MAJ         1
#define COOS_REV_MIN         6

//##############################################################################
// Template class                                              
//##############################################################################

template <unsigned char COOS_MAX_TASKS, char TIMING> class Coos{
  public:
                    Coos(void);                         // constructor
    void            register_task(void (*tsk)(void));   // user's tasks must be registered first
    void            start(void);                        // init scheduler once
    void            run(void);                          // COOS task switcher
    jmp_buf         main_context;                       // context of scheduler
    jmp_buf         task_context[COOS_MAX_TASKS];       // list of task contexts
    uchar           task_no;                            // current task No
    int             task_delay[COOS_MAX_TASKS];         // task delay in msec, task stopped if value is negative
    unsigned int    msec;                               // ms of current sec
    unsigned long   uptime;                             // seconds since start
    
  private:
    void            (*tsk_p[COOS_MAX_TASKS])(void);     // list of registered tasks
    unsigned int    ms;
    unsigned char   task_cnt;                           // counts registered coos tasks    
    void            update_time(void);
    unsigned char   stored_min;
};

//##############################################################################
// Implementation
//##############################################################################

// =================================
// Constructor
// =================================
template <unsigned char COOS_MAX_TASKS, char TIMING> Coos<COOS_MAX_TASKS, TIMING>::Coos(void)
{
  uptime = 0;
  msec = 0;
  ms = 0;
  task_cnt = 0;
  for (int i=0; i<COOS_MAX_TASKS; i++)
  {
    tsk_p[i] = NULL;                   // task is not registered
    task_delay[i] = COOS_STOP;         // all unregistered tasks stopped
  }
}
// =================================
// Register a task 
// =================================
template <unsigned char COOS_MAX_TASKS, char TIMING> void Coos<COOS_MAX_TASKS, TIMING>::register_task(void (*tsk)(void))
{
  if (task_cnt < COOS_MAX_TASKS)
  {
    tsk_p[task_cnt++] = tsk;
  }
}
// =================================
// Update time
// =================================
// supposed to happen more often than every millisecond,
// task should not keep control for more than about 900 us
template <unsigned char COOS_MAX_TASKS, char TIMING> void Coos<COOS_MAX_TASKS, TIMING>::update_time(void)
{
  unsigned int millisec = (unsigned int)millis(); 
    while (ms != millisec)  // catch up time
    {      
      ms++;  
      for (int i=0; i<task_cnt; i++)
      {
        if (task_delay[i] > 0)  // decrement positive delays 
        {
          task_delay[i]--;
        }
      }
      if (++msec >= 1000) // if 1 sec passed
      {
        uptime++;               // count seconds since start
        msec = 0;
      }
    }  
}
// =================================
// Start scheduler - init registered tasks
// =================================
template <unsigned char COOS_MAX_TASKS, char TIMING> void Coos<COOS_MAX_TASKS, TIMING>::start(void)
{
  int   res;
  void (*tsk)(void);
  update_time(); 
  for (task_no=0; task_no<task_cnt; task_no++)
  {
    if (tsk_p[task_no] != NULL)            // if task was registered
    {
      res = setjmp(main_context);
      if (res == 0)
      {
        tsk = tsk_p[task_no];
        tsk();                             // invoke task
      }
      else
      {
        task_delay[task_no] = --res;      // task returns the required delay
      }
    }  
    update_time(); 
  }
  task_no = 0;
}
// =================================
// Run scheduler on regular basis
// =================================
template <unsigned char COOS_MAX_TASKS, char TIMING> void Coos<COOS_MAX_TASKS, TIMING>::run(void)
{
  int res;
  int tmp;
  if (task_delay[task_no] == 0)
  {
    res = setjmp(main_context);           // set return point and get delay value from the task 
    if (res == 0)                         // after setting return point
    {
      longjmp(task_context[task_no], 1);  // invoke task 
    }
    else                                  // after returning from task 
    {
      tmp = --res;
      task_delay[task_no] = tmp;          // set task delay (negative delay - task stopped) 
    }
  }
  if (++task_no >= task_cnt)               // next task
  {
    task_no = 0;                          // round-robin
  }
  update_time(); 
}
#endif /* __COOS_H */
