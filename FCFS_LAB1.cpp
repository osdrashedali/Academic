#include <iostream>
using namespace std;

int main() {
    int n;

    cout << "Enter total number of processes: ";
    cin >> n;

    int bt[100], wt[100], tat[100];
    float total_wt = 0, total_tat = 0;

   //input nicchi
    for (int i = 0; i < n; i++) {
        cout << "Enter Burst Time for P" << i + 1 << ": ";
        cin >> bt[i];
    }
    //  Calculate Waiting Times
    wt[0] = 0; // First process waiting time = 0 hobe
    for (int i = 1; i < n; i++) {
        wt[i] = wt[i - 1] + bt[i - 1];
    }

    // cal tat and total
    for (int i = 0; i < n; i++) {
        tat[i] = wt[i] + bt[i];
        total_wt += wt[i];
        total_tat += tat[i];
    }
    // Scheduling 
    cout << "\nScheduling Order:\n ";
    for (int i = 0; i < n; i++) {
        cout << "P" << i + 1 << " -> "<< tat[i]<<endl;
    }

    cout << "\nAverage Waiting Time = " << total_wt / n << endl;
    cout << "Average Turnaround Time = " << total_tat / n << endl;

    return 0;
}