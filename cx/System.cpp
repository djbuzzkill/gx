
#include "System.h"
#include "Task.h"

namespace cx
{
   //
   void Destroy (Destructor* o)
      {
   if (o) delete o; 
      }


   //
   int Run_realtime_task (cx::SystemContext* sys, cx::RT_Task* task)
   {

      int run_state =0 ; 
      do switch (run_state)
      {
      case 0: 

         task->Initialize (sys); 
         run_state++; 
         break; 

      case 1: 
         while (sys->DispatchEvents ())
         {
            task->Update (0.0, sys); 
         }
         run_state++; 
         break;
      case 2: 
         task->Deinitialize (sys); 
         run_state++; 
         break;

      } while (run_state < 3); 

      return 0; 
   }
}
