
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace myos
{
   
   struct CPUState
   {
       common::uint32_t eax;
       common::uint32_t ebx;
       common::uint32_t ecx;
       common::uint32_t edx;

       common::uint32_t esi;
       common::uint32_t edi;
       common::uint32_t ebp;

       /*
       common::uint32_t gs;
       common::uint32_t fs;
       common::uint32_t es;
       common::uint32_t ds;
       */
       common::uint32_t error;

       common::uint32_t eip;
       common::uint32_t cs;
       common::uint32_t eflags;
       common::uint32_t esp;
       common::uint32_t ss;
   } __attribute__((packed));
   

   class Process{

   friend class ProcessManager;

   public:

       Process();
       Process(void(*_func)(void*));
       Process(GlobalDescriptorTable* gdt, void(*_func)(void*));
       ~Process();
       inline unsigned long int get(){return id;}
       bool setCpuState(GlobalDescriptorTable* gdt, void(*f)());
       void setCPUState(CPUState* cpustate);
       CPUState* getCPUState(){return cpustate;}
       void setState(int state);
       void setPC(int PC);
       int getId();

   private:

       static int static_id;
       int id;
       int status;
       int PC;
       CPUState* cpustate;
       common::uint8_t stack[4096];
};

class ProcessManager{
   
   public:
       ProcessManager();
       ProcessManager(Process* process);
       ~ProcessManager();
       inline int getNumProcess(){return processNumber;}
       inline int getCurrentProcessNum(){return currentProcess;}
       Process* getCurrentProcess();
       CPUState* getCurrentCPUState();
       bool addTProcess(Process* process);
       void setProcess(int num, Process* process);
       void setCurrentProcess(int _current);
       Process* getProcess(int index);
       int getProcessIdIndex(int id);
       Process* getProcess_id(int id);
       void delete_Process(Process* process);
       CPUState* Schedule(CPUState* cpustate);
       
   private:
       Process* process[256];
       int processNumber;
       int currentProcess;
};

class ProcessInfo{
   friend class ProcessTable;
   public:
       ProcessInfo();
       ProcessInfo(Process* process);
       ~ProcessInfo();
       void setParentProcessID(int parentId);
       void setState(int state);
       void setTableIndex(int i);
       int getTableIndex();
       int getState();
       int getParentId(){return parentProcessID;}
       CPUState* getProcessCPUState(){return processCpuState;}
       
       
   private:
       int pid;
       CPUState* processCpuState;
       int parentProcessID;
       int state_running;
       int state_wait;
       int state_terminated;
       int tableIndex;
};

   class ProcessTable{
       public:
           ProcessTable();
           ~ProcessTable();
           void addProcessInfo(ProcessInfo* process);
           void deleteProcessInfo(int index);
           ProcessInfo* getProcessInfo(int index);

       private:
           ProcessInfo* processInfo[256];
   };
   
   class Task
   {
   friend class TaskManager;
   private:
       common::uint8_t stack[4096]; // 4 KiB
       CPUState* cpustate;
       unsigned long int id;
       unsigned int size = 4096;
       int numProcess = 0;
   public:
       ProcessManager* processManager;
       Task();
       Task(GlobalDescriptorTable *gdt, Process* process);
       Task(ProcessManager* processManager);
       Task(GlobalDescriptorTable *gdt, void entrypoint());
       ~Task();
       Process* getCurrentProcess();
       void deleteProcess(Process* process);
       void addProcess(Process* process);
       void setProcessManager(ProcessManager* processManager);
       CPUState* Schedule(CPUState* cpustate);
   };
   
   
   class TaskManager
   {
   private:
       Task* tasks[256];
       int numTasks;
       int currentTask;
   public:
       TaskManager();
       ~TaskManager();
       bool AddTask(Task* task);
       CPUState* Schedule(CPUState* cpustate);
   };
   
   
}


#endif
