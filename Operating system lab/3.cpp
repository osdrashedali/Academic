
#include <iostream>
using namespace std;

int main()
{
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    int pid[100], at[100], bt[100], priority[100];
    int ct[100], tat[100], wt[100], done[100] = {0};

    for(int i = 0; i < n; i++)
    {
        pid[i] = i + 1;

        cout << "\nProcess P" << pid[i] << endl;

        cout << "Enter Arrival Time: ";
        cin >> at[i];

        cout << "Enter Burst Time: ";
        cin >> bt[i];

        cout << "Enter Priority: ";
        cin >> priority[i];
    }

    int time = 0;

    for(int i = 0; i < n; i++)
    {
        int k = -1;

        for(int j = 0; j < n; j++)
        {
            if(done[j] == 0 && at[j] <= time)
            {
                if(k == -1 || priority[j] < priority[k])
                {
                    k = j;
                }
            }
        }

        if(k == -1)
        {
            time++;
            i--;
            continue;
        }

        time = time + bt[k];
        ct[k] = time;
        tat[k] = ct[k] - at[k];
        wt[k] = tat[k] - bt[k];

        done[k] = 1;
    }

    cout << "\nPID\tAT\tBT\tPriority\tCT\tTAT\tWT\n";

    float avg_wt = 0, avg_tat = 0;

    for(int i = 0; i < n; i++)
    {
        cout << "P" << pid[i] << "\t"
             << at[i] << "\t"
             << bt[i] << "\t"
             << priority[i] << "\t\t"
             << ct[i] << "\t"
             << tat[i] << "\t"
             << wt[i] << endl;

        avg_wt += wt[i];
        avg_tat += tat[i];
    }

    cout << "\nAverage Waiting Time = " << avg_wt / n << endl;
    cout << "Average Turnaround Time = " << avg_tat / n << endl;

    return 0;
}
