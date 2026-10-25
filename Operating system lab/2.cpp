
#include <iostream>
using namespace std;

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    int p[100], at[100], bt[100];
    int wt[100], tat[100], ct[100], done[100] = {0};

    for (int i = 0; i < n; i++) {
        p[i] = i + 1;

        cout << "Enter Arrival Time of P" << p[i] << ": ";
        cin >> at[i];

        cout << "Enter Burst Time of P" << p[i] << ": ";
        cin >> bt[i];
    }

    int time = 0, completed = 0;

    while (completed < n) {
        int idx = -1;

        for (int i = 0; i < n; i++) {
            if (done[i] == 0 && at[i] <= time) {
                if (idx == -1 || bt[i] < bt[idx]) {
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            time++;
        } else {
            time += bt[idx];
            ct[idx] = time;
            tat[idx] = ct[idx] - at[idx];
            wt[idx] = tat[idx] - bt[idx];

            done[idx] = 1;
            completed++;
        }
    }

    double total_wt = 0, total_tat = 0;

    cout << "\nP\tAT\tBT\tCT\tWT\tTAT\n";

    for (int i = 0; i < n; i++) {
        cout << "P" << p[i] << "\t"
             << at[i] << "\t"
             << bt[i] << "\t"
             << ct[i] << "\t"
             << wt[i] << "\t"
             << tat[i] << endl;

        total_wt += wt[i];
        total_tat += tat[i];
    }

    cout << "\nAverage Waiting Time: "
         << total_wt / n << endl;

    cout << "Average Turnaround Time: "
         << total_tat / n << endl;

    return 0;
}
