#include <iostream>
#include <stdio.h>
#include "set"
#include <algorithm>
#include "map"
#include "string"
#include "iostream"
#include "list"
#include "queue"
#include "stack"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vector"
#include "fstream"
#include <regex>

using namespace std;

struct Program_Arguments {
    bool flag_v_enabled;
    bool flag_t_enabled;
    bool flag_e_enabled;
    bool flag_p_enabled;
    bool flag_s_enabled;
    string flag_s_value;

    Program_Arguments() {
        flag_v_enabled = false;
        flag_t_enabled = false;
        flag_e_enabled = false;
        flag_p_enabled = false;
        flag_s_enabled = false;
        flag_s_value = "";
    }
};

struct Process {
    int pid;
    int arrival_time;
    int total_cpu_time;
    int cpu_burst;
    int io_burst;
    int static_prio;
    int dynamic_prio;
    int finish_time;
    int tat;
    int io_wait_time;
    int cpu_wait_time;
    int state_ts;
    int end_running_time;
    int remaining_cpu_time;
    int remaining_burst_time;

    Process(int pid, int arrival_time, int total_cpu_time, int cpu_burst, int io_burst) {
        this->pid = pid;
        this->arrival_time = arrival_time;
        this->total_cpu_time = total_cpu_time;
        this->cpu_burst = cpu_burst;
        this->io_burst = io_burst;

        this->state_ts = arrival_time;
        this->end_running_time = state_ts;
        this->cpu_wait_time = 0;
        this->io_wait_time = 0;
        this->remaining_cpu_time = total_cpu_time;
        this->remaining_burst_time = 0;
    }
};

enum State {
    CREATED,
    READY,
    RUNNING,
    BLOCK,
    PREEMPT,
    DONE
};

struct Event {
    int timestamp;
    State prev_state;
    State new_state;
    Process *process;

    Event(int timestamp, State prev_state, State new_state, Process *process) {
        this->timestamp = timestamp;
        this->prev_state = prev_state;
        this->new_state = new_state;
        this->process = process;
    }
};

class DES {
private:

public:
    list<Event *> event_list;

    DES() {}

    void put_event(Event *event) {
        bool event_inserted = false;
        auto itr = event_list.begin();

        while (itr != event_list.end()) {
            if ((*itr)->timestamp > event->timestamp) {
                event_list.insert(itr, event);
                event_inserted = true;
                break;
            }
            itr++;
        }

        if (!event_inserted) {
            event_list.push_back(event);
        }
    }

    Event *get_event() {
        Event *event = nullptr;
        if (!event_list.empty()) {
            event = event_list.front();
            event_list.pop_front();
        }
        return event;
    }

    void rm_event(Process *process) {
        // remove event in the DES layer with process id = process.pid
        auto itr = event_list.begin();
        while (itr != event_list.end()) {
            if ((*itr)->process->pid == process->pid) {
                break;
            }
            itr++;
        }
        if (itr != event_list.end()) {
            event_list.erase(itr);
        }
    }

    int get_next_event_time() {
        if (event_list.empty()) {
            return -1;
        }
        return event_list.front()->timestamp;
    }
};

class Scheduler {
public:
    virtual void add_process(Process *process) = 0;

    virtual Process *get_next_process() = 0;

    virtual bool test_preempt(Process *current_running_process, Process *preempting_process, int current_time,
                              list<Event *> &event_list) = 0;

    virtual void print_name() = 0;

    Scheduler() {
        this->quantum = 10000;
        this->max_prio = 4;
    }

    int max_prio;
    int quantum;
};

class FCFS : public Scheduler {
private:
    queue<Process *> runQ;

public:

    FCFS() : Scheduler() {}

    void add_process(Process *process) {
        runQ.push(process);
    }

    Process *get_next_process() {
        if (runQ.empty()) {
            return nullptr;
        }
        Process *process = runQ.front();
        runQ.pop();
        return process;
    }

    void print_name() {
        cout << "FCFS" << endl;
    }

    bool test_preempt(Process *current_running_process, Process *preempting_process, int current_time,
                      list<Event *> &event_list) {
        return false;
    }

};

class LCFS : public Scheduler {
private:
    stack<Process *> run_proc_stack;

public:

    LCFS() : Scheduler() {}

    void add_process(Process *process) {
        run_proc_stack.push(process);
    }

    Process *get_next_process() {
        if (run_proc_stack.empty()) {
            return nullptr;
        }
        Process *process = run_proc_stack.top();
        run_proc_stack.pop();
        return process;
    }

    void print_name() {
        cout << "LCFS" << endl;
    }

    bool test_preempt(Process *current_running_process, Process *preempting_process, int current_time,
                      list<Event *> &event_list) {
        return false;
    }

};

class SRTF : public Scheduler {
private:
    list<Process *> runQ;

public:
    SRTF() : Scheduler() {}

    void add_process(Process *process) {
        bool process_inserted = false;
        auto itr = runQ.begin();

        while (itr != runQ.end()) {
            if ((*itr)->remaining_cpu_time > process->remaining_cpu_time) {
                runQ.insert(itr, process);
                process_inserted = true;
                break;
            }
            itr++;
        }

        if (!process_inserted) {
            runQ.push_back(process);
        }
    }

    Process *get_next_process() {
        if (runQ.empty()) {
            return nullptr;
        }
        Process *process = runQ.front();
        runQ.pop_front();
        return process;
    }

    void print_name() {
        cout << "SRTF" << endl;
    }

    bool test_preempt(Process *current_running_process, Process *preempting_process, int current_time,
                      list<Event *> &event_list) {
        return false;
    }
};

class RR : public Scheduler {
private:
    queue<Process *> runQ;

public:

    RR(int quantum) : Scheduler() {
        this->quantum = quantum;
    }

    void add_process(Process *process) {
        runQ.push(process);
    }

    Process *get_next_process() {
        if (runQ.empty()) {
            return nullptr;
        }
        Process *process = runQ.front();
        runQ.pop();
        return process;
    }

    void print_name() {
        cout << "RR" << " " << quantum << endl;
    }

    bool test_preempt(Process *current_running_process, Process *preempting_process, int current_time,
                      list<Event *> &event_list) {
        return false;
    }

};

class PRIO : public Scheduler {
private:
    vector<queue<Process *> > *activeQ;
    vector<queue<Process *> > *expiredQ;

public:
    PRIO(int quantum, int max_prio) : Scheduler() {
        this->quantum = quantum;
        this->max_prio = max_prio;

        activeQ = new vector<queue<Process *> >();
        expiredQ = new vector<queue<Process *> >();

        for (int i = 0; i < max_prio; i++) {
            activeQ->push_back(queue<Process *>());
            expiredQ->push_back(queue<Process *>());
        }
    }

    void add_process(Process *process) {
        if (process->remaining_burst_time > 0) {
            process->dynamic_prio--;
            // if the dynamic_prio == -1, we need to add the process to the expired queue with new dynamic_prio = static_prio - 1
            if (process->dynamic_prio == -1) {
                process->dynamic_prio = process->static_prio - 1;
                (*expiredQ)[process->dynamic_prio].push(process);
                return;
            }
        } else {
            process->dynamic_prio = process->static_prio - 1;
        }
        (*activeQ)[process->dynamic_prio].push(process);
    }

    Process *get_next_process() {

        Process *next_process = nullptr;

        for (auto itr = (*activeQ).rbegin(); itr != (*activeQ).rend(); itr++) {
            if (!(*itr).empty()) {
                next_process = (*itr).front();
                (*itr).pop();
                return next_process;
            }
        }

        swap(activeQ, expiredQ);

        for (auto itr = (*activeQ).rbegin(); itr != (*activeQ).rend(); itr++) {
            if (!(*itr).empty()) {
                next_process = (*itr).front();
                (*itr).pop();
                return next_process;
            }
        }

        return next_process;

    }

    void print_name() {
        cout << "PRIO " << quantum << endl;
    }

    bool test_preempt(Process *current_running_process, Process *preempting_process, int current_time,
                      list<Event *> &event_list) {
        return false;
    }

};

class PREPRIO : public Scheduler {
private:
    vector<queue<Process *> > *activeQ;
    vector<queue<Process *> > *expiredQ;

public:
    PREPRIO(int quantum, int max_prio) : Scheduler() {
        this->quantum = quantum;
        this->max_prio = max_prio;

        activeQ = new vector<queue<Process *> >();
        expiredQ = new vector<queue<Process *> >();

        for (int i = 0; i < max_prio; i++) {
            activeQ->push_back(queue<Process *>());
            expiredQ->push_back(queue<Process *>());
        }
    }

    void add_process(Process *process) {
        if (process->remaining_burst_time > 0) {
            process->dynamic_prio--;
            // if the dynamic_prio == -1, we need to add the process to the expired queue with new dynamic_prio = static_prio - 1
            if (process->dynamic_prio == -1) {
                process->dynamic_prio = process->static_prio - 1;
                (*expiredQ)[process->dynamic_prio].push(process);
                return;
            }
        } else {
            process->dynamic_prio = process->static_prio - 1;
        }
        (*activeQ)[process->dynamic_prio].push(process);
    }

    Process *get_next_process() {

        Process *next_process = nullptr;

        for (auto itr = (*activeQ).rbegin(); itr != (*activeQ).rend(); itr++) {
            if (!(*itr).empty()) {
                next_process = (*itr).front();
                (*itr).pop();
                return next_process;
            }
        }

        swap(activeQ, expiredQ);

        for (auto itr = (*activeQ).rbegin(); itr != (*activeQ).rend(); itr++) {
            if (!(*itr).empty()) {
                next_process = (*itr).front();
                (*itr).pop();
                return next_process;
            }
        }

        return next_process;

    }

    void print_name() {
        cout << "PREPRIO " << quantum << endl;
    }

    bool timestamp_check(int time, int pid, list<Event *> event_list) {
        for (auto itr = event_list.begin(); itr != event_list.end(); itr++) {
            if ((*itr)->process->pid == pid && (*itr)->timestamp == time) {
                return false;
            }
        }
        return true;
    }

    bool test_preempt(Process *current_running_process, Process *preempting_process, int current_time,
                      list<Event *> &event_list) {
        return preempting_process->dynamic_prio > current_running_process->dynamic_prio &&
               timestamp_check(current_time, current_running_process->pid, event_list);
    }

};

class Simulator {
private:
    DES *des;
    Scheduler *scheduler;
    queue<Process *> processes;
    int ofs;
    int num_of_random_nos;
    int *randvals;

    int time_cpu_busy;
    int time_io_busy;

    int start_of_io_utilisation;
    int processes_in_blocked_state;
    Program_Arguments program_args;

    void init_random_array(istream &random_file) {
        random_file >> num_of_random_nos;
        randvals = new int[num_of_random_nos];

        int num;
        for (int i = 0; i < num_of_random_nos; i++) {
            random_file >> num;
            randvals[i] = num;
        }
    }

    void create_processes(istream &input_file) {
        int arrival_time, total_cpu, cpu_burst, io_burst;
        int pid = 0;

        while (input_file >> arrival_time >> total_cpu >> cpu_burst >> io_burst) {
            Process *process = new Process(pid, arrival_time, total_cpu, cpu_burst, io_burst);
            process->static_prio = get_random_number(scheduler->max_prio);
            process->dynamic_prio = process->static_prio - 1;
            Event *event = new Event(arrival_time, CREATED, READY, process);
            des->put_event(event);
            processes.push(process);
            pid++;
        }
    }

    string enum_to_string(State state) {
        string s;
        switch (state) {
            case CREATED:
                s = "CREATED";
                break;
            case READY:
                s = "READY";
                break;
            case RUNNING:
                s = "RUNNG";
                break;
            case BLOCK:
                s = "BLOCK";
                break;
            case DONE:
                s = "DONE";
                break;
            default:
                s = "UNKNOWN";
        }

        return s;
    }

    void print_event(int timestamp, Process *proc, int curr_state_time, State prev_state, State new_state) {
        if (program_args.flag_v_enabled) {
            printf("%d %d %d: %s -> %s", timestamp, proc->pid, curr_state_time, enum_to_string(prev_state).c_str(),
                   enum_to_string(new_state).c_str());
            printf(" total_cpu_rem=%d burst_rem=%d prio=%d\n", proc->remaining_cpu_time, proc->remaining_burst_time,
                   proc->dynamic_prio);
        }
    }

public:
    Simulator(DES *des, Scheduler *scheduler, istream &input_file, istream &random_file,
              Program_Arguments program_args) {
        this->des = des;
        this->scheduler = scheduler;
        this->ofs = 0;
        this->num_of_random_nos = 0;
        this->time_cpu_busy = 0;
        this->time_io_busy = 0;
        this->start_of_io_utilisation = 0;
        this->processes_in_blocked_state = 0;
        this->program_args = program_args;
        init_random_array(random_file);
        create_processes(input_file);
    }

    int get_random_number(int burst) {
        int val = 1 + (randvals[ofs] % burst);
        ofs++;
        if (ofs == num_of_random_nos) {
            ofs = 0;
        }
        return val;
    }

    void print_summary() {
        // summary attributes
        scheduler->print_name();
        int num_processes = processes.size();
        double avg_tat = 0;
        double avg_cpu_wait_time = 0;
        int finishing_time_of_last_event = 0;
        double throughput_per_100_time_units = 0;
        double cpu_utilization = 0;
        double io_utilization = 0;

        while (!processes.empty()) {
            Process *process = processes.front();
            avg_tat += (double) process->tat;
            avg_cpu_wait_time += (double) process->cpu_wait_time;
            finishing_time_of_last_event = max(finishing_time_of_last_event, process->finish_time);
            printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
                   process->pid,
                   process->arrival_time,
                   process->total_cpu_time,
                   process->cpu_burst,
                   process->io_burst,
                   process->static_prio,
                   process->finish_time,
                   process->tat,
                   process->io_wait_time,
                   process->cpu_wait_time
            );
            processes.pop();
        }

        avg_tat /= (double) num_processes;
        avg_cpu_wait_time /= (double) num_processes;

        cpu_utilization = 100 * time_cpu_busy / (double) finishing_time_of_last_event;
        io_utilization = 100 * time_io_busy / (double) finishing_time_of_last_event;

        throughput_per_100_time_units = 100 * num_processes / (double) finishing_time_of_last_event;

        printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
               finishing_time_of_last_event,
               cpu_utilization,
               io_utilization,
               avg_tat,
               avg_cpu_wait_time,
               throughput_per_100_time_units
        );

    }

    void simulate() {
        Event *evt;
        int CURRENT_TIME;

        Process *CURRENT_RUNNING_PROCESS = nullptr;
        bool CALL_SCHEDULER = false;

        while ((evt = des->get_event())) {
            Process *proc = evt->process;
            CURRENT_TIME = evt->timestamp;
            State transition_from = evt->prev_state;
            State transition_to = evt->new_state;

            print_event(CURRENT_TIME, proc, (CURRENT_TIME - proc->state_ts), transition_from, transition_to);

            if (transition_from == READY) {
                proc->cpu_wait_time += CURRENT_TIME - proc->state_ts;
            }

            if (transition_from == BLOCK) {
                proc->io_wait_time += CURRENT_TIME - proc->state_ts;
                processes_in_blocked_state--;
                if (processes_in_blocked_state == 0) {
                    time_io_busy += CURRENT_TIME - start_of_io_utilisation;
                }
            }

            delete evt;
            evt = nullptr; // remove cur event obj and don’t touch anymore

            Event *new_event = nullptr;

            switch (transition_to) {  // encodes where we come from and where we go
                case READY: {
                    // must come from BLOCKED or CREATED
                    // add to run queue, no event created

                    scheduler->add_process(proc);

                    int delta;

                    if (transition_from == RUNNING) {
                        CURRENT_RUNNING_PROCESS = nullptr;
                    } else if ((transition_from == CREATED || transition_from == BLOCK) &&
                               CURRENT_RUNNING_PROCESS != nullptr) {
                        bool PREEMPT_CURRENT_PROCESS = scheduler->test_preempt(CURRENT_RUNNING_PROCESS, proc,
                                                                               CURRENT_TIME, des->event_list);
                        if (PREEMPT_CURRENT_PROCESS) {
                            // remove future event for the preempted process from DES.
                            des->rm_event(CURRENT_RUNNING_PROCESS);
                            new_event = new Event(CURRENT_TIME, RUNNING, READY, CURRENT_RUNNING_PROCESS);
                            des->put_event(new_event);
                            delta = CURRENT_RUNNING_PROCESS->end_running_time - CURRENT_TIME;
                            CURRENT_RUNNING_PROCESS->end_running_time = CURRENT_TIME;
                            CURRENT_RUNNING_PROCESS->remaining_cpu_time += delta;
                            CURRENT_RUNNING_PROCESS->remaining_burst_time += delta;
                            time_cpu_busy -= delta;
                        }
                    }
                    CALL_SCHEDULER = true;
                    break;
                }
                case RUNNING: {
                    // create event for either preemption or blocking

                    int cpu_burst_duration;

                    // remaining burst time can never exceed quantum
                    if (proc->remaining_burst_time > 0) {
                        // process can goto either DONE, READY, or BLOCK state

                        if (proc->remaining_burst_time > scheduler->quantum) { // create event for PREEMPTION
                            proc->remaining_burst_time -= scheduler->quantum;
                            cpu_burst_duration = scheduler->quantum;
                            proc->remaining_cpu_time -= cpu_burst_duration;
                            new_event = new Event(CURRENT_TIME + cpu_burst_duration, RUNNING, READY, proc);
                            des->put_event(new_event);
                        } else if (proc->remaining_burst_time ==
                                   proc->remaining_cpu_time) { // create event for DONE state
                            cpu_burst_duration = proc->remaining_cpu_time;
                            new_event = new Event(CURRENT_TIME + cpu_burst_duration, RUNNING, DONE, proc);
                            des->put_event(new_event);
                            proc->remaining_burst_time = 0;
                            proc->remaining_cpu_time = 0;
                        } else { // Create event to BLOCKED state
                            cpu_burst_duration = proc->remaining_burst_time;
                            new_event = new Event(CURRENT_TIME + cpu_burst_duration, RUNNING, BLOCK, proc);
                            des->put_event(new_event);
                            proc->remaining_cpu_time -= cpu_burst_duration;
                            proc->remaining_burst_time = 0;
                        }

                    } else {
                        // process can goto either to DONE, PREEMPT, BLOCK state

                        int new_cpu_burst = get_random_number(proc->cpu_burst);

                        if (new_cpu_burst > proc->remaining_cpu_time) {
                            new_cpu_burst = proc->remaining_cpu_time;
                        }

                        if (new_cpu_burst > scheduler->quantum) {
                            cpu_burst_duration = scheduler->quantum;
                            // Create an event for PREEMPTION
                            new_event = new Event(CURRENT_TIME + cpu_burst_duration, RUNNING, READY, proc);
                            des->put_event(new_event);
                            proc->remaining_burst_time = new_cpu_burst - cpu_burst_duration;
                            proc->remaining_cpu_time -= cpu_burst_duration;
                        } else {
                            cpu_burst_duration = new_cpu_burst;
                            // Create event for either DONE or BLOCKED state
                            if (proc->remaining_cpu_time <= cpu_burst_duration) {
                                cpu_burst_duration = proc->remaining_cpu_time;
                                new_event = new Event(CURRENT_TIME + cpu_burst_duration, RUNNING, DONE, proc);
                                des->put_event(new_event);
                                proc->remaining_burst_time = 0;
                                proc->remaining_cpu_time = 0;
                            } else {
                                // Create block event RUNNING -> BLOCKED
                                new_event = new Event(CURRENT_TIME + cpu_burst_duration, RUNNING, BLOCK, proc);
                                des->put_event(new_event);
                                proc->remaining_cpu_time -= cpu_burst_duration;
                            }
                        }
                    }

                    time_cpu_busy += cpu_burst_duration;
                    proc->end_running_time = CURRENT_TIME + cpu_burst_duration;

                    break;
                }

                case BLOCK: {
                    //create an event for when process becomes READY again
                    processes_in_blocked_state++;
                    if (processes_in_blocked_state == 1) {
                        start_of_io_utilisation = CURRENT_TIME;
                    }

                    int io_burst_duration = get_random_number(proc->io_burst);

                    CURRENT_RUNNING_PROCESS = nullptr;
                    new_event = new Event(CURRENT_TIME + io_burst_duration, BLOCK, READY, proc);

                    des->put_event(new_event);
                    CALL_SCHEDULER = true;
                    break;
                }

                case CREATED:
                    break;

                case DONE: {
                    CURRENT_RUNNING_PROCESS = nullptr;
                    proc->finish_time = CURRENT_TIME;
                    proc->tat = proc->finish_time - proc->arrival_time;
                    CALL_SCHEDULER = true;
                    break;
                }
            }

            proc->state_ts = CURRENT_TIME;

            if (CALL_SCHEDULER) {
                if (des->get_next_event_time() == CURRENT_TIME)
                    continue; //process next event from Event queue
                CALL_SCHEDULER = false; // reset global flag
                if (CURRENT_RUNNING_PROCESS == nullptr) {
                    CURRENT_RUNNING_PROCESS = scheduler->get_next_process();
                    if (CURRENT_RUNNING_PROCESS == nullptr)
                        continue;
                    // create event to make this process runnable for same time.
                    new_event = new Event(CURRENT_TIME, READY, RUNNING, CURRENT_RUNNING_PROCESS);
                    des->put_event(new_event);
                }
            }
        }
    }

};

int main(int argc, char *argv[]) {
    int c;

    // read command line args
    Program_Arguments program_arguments;

    while ((c = getopt(argc, argv, "vteps:")) != -1) {
        switch (c) {
            case 'v':
                program_arguments.flag_v_enabled = true;
                break;
            case 't':
                program_arguments.flag_t_enabled = true;
                break;
            case 'e':
                program_arguments.flag_e_enabled = true;
                break;
            case 'p':
                program_arguments.flag_p_enabled = true;
                break;
            case 's':
                program_arguments.flag_s_enabled = true;
                program_arguments.flag_s_value = optarg;
                break;
        }
    }

    if (!program_arguments.flag_s_enabled) {
        cout << "Error: scheduler is not specified" << endl;
        return 1;
    }

    Scheduler *scheduler;
    DES *des = new DES();

    switch (program_arguments.flag_s_value[0]) {
        case 'F':
            scheduler = new FCFS();
            break;
        case 'L':
            scheduler = new LCFS();
            break;
        case 'S':
            scheduler = new SRTF();
            break;
        case 'R': {
            string quantum = program_arguments.flag_s_value.substr(1);
            try {
                scheduler = new RR(stoi(quantum));
            } catch (...) {
                cout << "Error: Quantum should be a valid number" << endl;
                return 1;
            }
            break;
        }
        case 'P':
        case 'E': {
            int char_pos = program_arguments.flag_s_value.find(':');
            int time_quantum = stoi(program_arguments.flag_s_value.substr(1));
            int max_prio = 4;
            if (char_pos != string::npos) {
                time_quantum = stoi(program_arguments.flag_s_value.substr(1, char_pos - 1));
                max_prio = stoi(program_arguments.flag_s_value.substr(char_pos + 1));
            }
            if (program_arguments.flag_s_value[0] == 'P') {
                scheduler = new PRIO(time_quantum, max_prio);
            } else {
                scheduler = new PREPRIO(time_quantum, max_prio);
            }
            break;
        }
        default:
            cout << "Error: Incorrect scheduler specified" << endl;
            break;
    }

    if (argc - optind < 2) {
        cout << "Error: insufficient args provided" << endl;
        return 1;
    }

    ifstream input_file(argv[optind]);
    ifstream random_file(argv[optind + 1]);

    Simulator *simulator = new Simulator(des, scheduler, input_file, random_file, program_arguments);
    simulator->simulate();
    simulator->print_summary();

    return 0;
}
