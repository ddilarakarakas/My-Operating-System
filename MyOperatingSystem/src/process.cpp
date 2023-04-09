#include <multitasking.h>

using namespace myos;
using namespace myos::common;

void printf2(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void printfHex2(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf2(foo);
}

/*
        PROCESS
 */

int Process::static_id = 0;
Process::Process(){
    id = static_id++;
}

Process::Process(GlobalDescriptorTable* gdt, void(*_func)(void*)){

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
    cpustate -> eip = (uint32_t)_func;
    cpustate -> cs = gdt->CodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;

    id = static_id++;
}

Process::~Process(){

}

void Process::setCPUState(CPUState* cpustate_){
    cpustate = cpustate_;
}

bool Process::setCpuState(GlobalDescriptorTable* gdt, void(*f)()){

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
    cpustate -> eip = (uint32_t)f;
    cpustate -> cs = gdt->CodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;

    return true;
}

void Process::setState(int _status){
    status = _status;
}

void Process::setPC(int _PC){
    PC = _PC;
}

/*
    PROCESS MANAGER
 */


ProcessManager::ProcessManager(){

}

ProcessManager::ProcessManager(Process* process){
    addProcess(process);
    currentProcess = -1;
}

ProcessManager::~ProcessManager(){

}

Process* ProcessManager::getCurrentProcess(){
    return processes[currentProcess];
}

CPUState* ProcessManager::getCurrentCPUState(){
    return processes[currentProcess]->getCPUState();
}

bool ProcessManager::addProcess(Process* process){
    if(processNumber >= 256){
        return false;
    }
    processNumber++;
    processes[processNumber] = process;
    return true;
}

void ProcessManager::setProcess(int index, Process* process){
    processes[index] = process;
}

void ProcessManager::setCurrentProcess(int current){
    currentProcess = current;
}

Process* ProcessManager::getProcess(int index){
    Process tempProcess;
    if(processNumber < index){
        return  &tempProcess;
    }
    return processes[index];
}

int ProcessManager:: getProcessIdIndex(int id){
    int i = 0;
    while(i < processNumber){
        if(processes[i]->getId() == id){
            return i;
        }
        i++;
    }
    return -1;
}

Process* ProcessManager::getProcess_id(int id,Process* tempProcess){
    int i = 0;
    while(i < processNumber){
        if(processes[i]->getId() == id){
            return processes[i];
        }
        i++;
    }
    return tempProcess;
}

void ProcessManager::deleteProcess(Process* process){
    Process tempProcess;
    for(int i=0;i<processNumber;i++){
        if(processes[i]->getId() == process->getId()){
            while(i<processNumber-1){
                processes[i] = processes[i+1];
                i++;
            }
            processes[i] = &tempProcess;
        }
    }
    processNumber--;
}

CPUState* ProcessManager::Schedule(CPUState* cpuState){
    if(processNumber <= 0){
        return cpuState;
    }
    if(currentProcess < processNumber-1){
        currentProcess++;
    }
    else
        currentProcess = 0;




    return processes[currentProcess]->getCPUState();
}


/*
 
        PROCESS INFO
 
 */


ProcessInfo::ProcessInfo(){

}

ProcessInfo::ProcessInfo(Process* process){
    pid = process->getId();
    processCpuState = process->getCPUState();
    state_running = 0;
    state_wait = 0;
    state_terminated = 0;
}

ProcessInfo::~ProcessInfo(){

}

void ProcessInfo::setParentProcessID(int parentID){
    parentProcessID = parentID;
}


void ProcessInfo::setState(int state){
    if(state == 0){ // State 0 -> Running
        state_running = 1;
        state_wait = 0;
        state_terminated = 0;
    }
    else if(state == 1){ // State 1 -> Wait
        state_running = 0;
        state_wait = 1;
        state_terminated = 0;
    }
    else if(state = 2){ // State 2 -> terminated;
        state_running = 0;
        state_wait = 0;
        state_terminated = 1;
    }
}

void ProcessInfo::setTableIndex(int index){
    tableIndex = index;
}

/*
        PROCESS TABLE
 */


ProcessTable::ProcessTable(){
    currentIndex = 0;
}

ProcessTable::~ProcessTable(){

}

void ProcessTable::addProcessInfo(ProcessInfo* process){
    processInfos[currentIndex] = process;
    currentIndex++;
}

void ProcessTable::deleteProcessInfo(int pid){
    if(currentIndex == 1){
        if(processInfos[0]->getPID() == pid){
            currentIndex--;
        }
    }
    for(int i=0;i<currentIndex;i++){
        if(processInfos[i]->getPID() == pid){
            while(i<currentIndex-2){
                processInfos[i] = processInfos[i+1];
                i++;
            }
            currentIndex = i;
        }
    }
}

ProcessInfo* ProcessTable::getProcessInfo(int index){
    return processInfos[index];
}
