
#include <multitasking.h>

using namespace myos;
using namespace myos::common;

/*
 
    TASK
 
 */

Task::Task(GlobalDescriptorTable *gdt, Process* process){
    processManager->addProcess(process);
    cpustate = process->getCPUState();
}

Task::Task(ProcessManager* processManagerr){
    Thread t;
    processManager = processManagerr;
    numProcess = processManagerr->getNumProcess();
    cpustate = processManagerr->getProcess(0,t)->getCPUState();
}

Process* Task::getCurrentProcess(){
    return processManager->getCurrentProcess();
}

void Task::deleteProcess(Process* process){
    Process temp;
    processManager->deleteProcess(process,temp);
    numProcess--;
}

void Task::addProcess(Process* process){
    processManager->addProcess[process];
    numProcess++;
}

void Task::setProcessManager(ProcessManager* manager){
    processManager = manager;
}

CPUState* Task::Schedule(CPUState* cpustate){
    if(numProcess <= 0)
        return cpustate;
    int tempCurrentProcess = processManager->getCurrentProcessNum();
    if(tempCurrentProcess >= 0){
        cpustate = processManager->getProcess(tempCurrentProcess)->getCPUState();
    }
    if(++tempCurrentProcess >= 0)
        processManager->setCurrentProcess(tempCurrentProcess %= numProcess);
    return cpustate;
}

Task::Task(GlobalDescriptorTable *gdt, void entrypoint())
{
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;

    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    
    /*
    cpustate -> gs = 0;
    cpustate -> fs = 0;
    cpustate -> es = 0;
    cpustate -> ds = 0;
    */
    
    // cpustate -> error = 0;
   
    // cpustate -> esp = ;
    cpustate -> eip = (uint32_t)entrypoint;
    cpustate -> cs = gdt->CodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;
    
}

Task::~Task()
{
}

/*
 
        TASK MANAGER
 
 */
        
TaskManager::TaskManager()
{
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{
}

bool TaskManager::AddTask(Task* task)
{
    if(numTasks >= 256)
        return false;
    tasks[numTasks++] = task;
    return true;
}

CPUState* TaskManager::Schedule(CPUState* cpustate)
{
    if(numTasks <= 0)
        return cpustate;
    
    if(currentTask >= 0)
        tasks[currentTask]->getCurrentProcess()->setCPUState(cpustate);
    
    if(++currentTask >= numTasks)
        currentTask %= numTasks;
    
    return tasks[currentTask]->processManager->Schedule(cpustate);
}

    
