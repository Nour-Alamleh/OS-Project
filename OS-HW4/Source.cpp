#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <climits>
using namespace std;
struct process {
    string name;  // process name
    int At;       // arrival time
    int Bt;       // burst time
    int Wt;       // waiting time
    int Tat;      // turnaround time
    int Ct;       // completion time
    int Rt;       // remaining time
};
bool compareAT(const process& p1, const process& p2) {
    return p1.At < p2.At;
}
// Function for FCFS Scheduling
void FCFS(vector<process>& procarray) {
    sort(procarray.begin(), procarray.end(), compareAT);
    int sum = 0;
    sum += procarray[0].At;
    for (int i = 0; i < procarray.size(); i++) {
        sum += procarray[i].Bt;
        procarray[i].Ct = sum;
        procarray[i].Tat = procarray[i].Ct - procarray[i].At;
        procarray[i].Wt = procarray[i].Tat - procarray[i].Bt;
        if (i < procarray.size() - 1 && sum < procarray[i + 1].At)
        {
            int idle = procarray[i + 1].At - sum;
            sum += idle;
        }
    }
    cout << "P\tAT\tBT\tWT\tTAT\tCT\n";
    for (const auto& proc : procarray) {
        cout << proc.name << "\t " << proc.At << "\t" << proc.Bt << "\t" << proc.Wt << "\t " << proc.Tat << "\t" << proc.Ct << "\n";
    }
    cout << "___________________________________________" << endl;
    // Draw Gantt Chart
    cout << "Gantt Chart:\n";
    vector<int> time;
    int lastCompletion = 0;
    for (const auto& proc : procarray) {
        if (proc.At > lastCompletion) {
            cout << "|  Idle  ";
            time.push_back(proc.At);
        }
        cout << "|  " << proc.name << "   ";
        time.push_back(proc.Ct);
        lastCompletion = proc.Ct;
    }
    cout << "|\n";
    cout << "0";
    for (const auto& time : time) {
        cout << "       " << time;
    }
    cout << endl;
}
// Function for SRTF Scheduling
void SRTF(vector<process>& procarray) {
    int n = procarray.size();
    vector<int> remaining_time(n);
    for (int i = 0; i < n; i++) {
        remaining_time[i] = procarray[i].Bt;
    }
    int completed = 0/*#compleated processes*/ ,current_time = 0, min_index = -1;
    vector<string> gantt_chart;
    while (completed < n) {
        int min_remaining_time = INT_MAX;
        // Find the process with the shortest remaining time at the current time
        for (int i = 0; i < n; i++) {
            if (procarray[i].At <= current_time && remaining_time[i] > 0 && remaining_time[i] < min_remaining_time) {
                min_remaining_time = remaining_time[i];
                min_index = i;
            }
        }
        if (min_index == -1) {
            gantt_chart.push_back("Idle");
            current_time++;
            continue;
        }
        // Execute the process
        gantt_chart.push_back(procarray[min_index].name);
        remaining_time[min_index]--;
        current_time++;
        // If the process is completed
        if (remaining_time[min_index] == 0) {
            completed++;
            procarray[min_index].Ct = current_time;
            procarray[min_index].Tat = procarray[min_index].Ct - procarray[min_index].At;
            procarray[min_index].Wt = procarray[min_index].Tat - procarray[min_index].Bt;
        }
    }
    cout << "P\tAT\tBT\tWT\tTAT\tCT\n";
    for (const auto& proc : procarray) {
        cout << proc.name << "\t " << proc.At << "\t" << proc.Bt << "\t" << proc.Wt << "\t " << proc.Tat << "\t" << proc.Ct << "\n";
    }
    cout << "___________________________________________" << endl;
    // Print the Gantt Chart
    cout << "Gantt Chart:\n";
    for (const auto& proc : gantt_chart) {
        cout << "|  " << proc << "  ";
    }
    cout << "|\n";
    // Print the timeline
    cout << "0";
    int time = 0;
    for (const auto& proc : gantt_chart) {
        time++;
        cout << "      " << time;
    }
    cout << endl;
}
// Function for Round Robin Scheduling
void RR(vector<process>& procarray, int tq) {
    vector<string> gantt_chart; // To store the sequence of execution
    vector<int> remaining_time(procarray.size()); // Remaining burst time for each process
    vector<int> timeline; // To record the timeline of Gantt Chart
    int current_time = 0; // Current time
    int completed = 0; // Number of completed processes
    queue<int> ready_queue; // Ready queue for Round Robin
    // Initialize remaining times
    for (int i = 0; i < procarray.size(); i++) {
        remaining_time[i] = procarray[i].Bt;
    }
    // Sort processes by arrival time
    sort(procarray.begin(), procarray.end(), compareAT);
    // Add the first processes to the queue if it arrives at time 0
    for (int i = 0; i < procarray.size(); i++) {
        if (procarray[i].At <= current_time) {
            ready_queue.push(i);
        }
    }
    while (completed < procarray.size()) {
        if (ready_queue.empty()) {
            gantt_chart.push_back("Idle");
            timeline.push_back(current_time);
            current_time++;
            // Check for new arrivals
            for (int i = 0; i < procarray.size(); i++) {
                if (procarray[i].At == current_time) {
                    ready_queue.push(i);
                }
            }
            continue;
        }
        // Process from the ready queue
        int current_index = ready_queue.front();
        ready_queue.pop();
        // Execute the process for min(time quantum, remaining time)
        gantt_chart.push_back(procarray[current_index].name);
        timeline.push_back(current_time);
        int execution_time = min(tq, remaining_time[current_index]);
        current_time += execution_time;
        remaining_time[current_index] -= execution_time;
        // Check for new arrivals during execution
        for (int i = 0; i < procarray.size(); i++) {
            if (procarray[i].At > timeline.back() && procarray[i].At <= current_time && remaining_time[i] > 0) {
                ready_queue.push(i);
            }
        }
        // If the process is not finished, add it back to the queue
        if (remaining_time[current_index] > 0) {
            ready_queue.push(current_index);
        }
        else {
            // Process finished
            completed++;
            procarray[current_index].Ct = current_time;
            procarray[current_index].Tat = procarray[current_index].Ct - procarray[current_index].At;
            procarray[current_index].Wt = procarray[current_index].Tat - procarray[current_index].Bt;
        }
    }
    timeline.push_back(current_time); // Final time for Gantt Chart
    // Output process details
    cout << "P\tAT\tBT\tWT\tTAT\tCT\n";
    for (const auto& proc : procarray) {
        cout << proc.name << "\t" << proc.At << "\t" << proc.Bt << "\t" << proc.Wt << "\t" << proc.Tat << "\t" << proc.Ct << "\n";
    }
    cout << "___________________________________________" << endl;
    // Print Gantt Chart
    cout << "Gantt Chart:\n";
    for (const auto& proc : gantt_chart) {
        cout << "|  " << proc << "  ";
    }
    cout << "|\n";
    // Print timeline
    cout << timeline[0];
    for (size_t i = 1; i < timeline.size(); i++) {
        cout << "      " << timeline[i];
    }
    cout << endl;
}
int main() {
    string filepath, choice;
    cout << "Enter the path to the file containing process data: ";
    cin >> filepath;
    ifstream fin(filepath);
    if (!fin.is_open()) {
        cout << "Error: Could not open the file.\n";
        return 1;
    }
    vector<process> procarray;
    string name;
    int at, bt;
    while (fin >> name >> at >> bt) {
        process proc;
        proc.name = name;
        proc.At = at;
        proc.Bt = bt;
        proc.Wt = proc.Tat = proc.Ct = proc.Rt = 0;
        procarray.push_back(proc);
    }
    do {
        cout << "Choose an algorithm (FCFS, SRTF, RR, EXIT): ";
        cin >> choice;
        if (choice == "FCFS") {
            FCFS(procarray);
        }
        else if (choice == "SRTF") {
            SRTF(procarray);
        }
        else if (choice == "RR") {
            int tq;
            cout << "Enter the time quantum: ";
            cin >> tq;
            RR(procarray, tq);
        }
        else if (choice == "EXIT") {
            cout << "Exiting program.\n";
        }
        else {
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != "EXIT");
    return 0;
}
