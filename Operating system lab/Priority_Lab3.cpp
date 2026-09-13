#include <iostream>
using namespace std;

int main()
{
    int n;

    cout << "Enter number of processes: ";
    cin >> n;

    int pid[100], bt[100], priority[100];
    int ct[100], tat[100], wt[100];

    for(int i = 0; i < n; i++)
    {
        pid[i] = i + 1;

        cout << "\nProcess P" << pid[i] << endl;

        cout << "Enter Burst Time: ";
        cin >> bt[i];

        cout << "Enter Priority: ";
        cin >> priority[i];
    }

    for(int i = 0; i < n - 1; i++)
    {
        for(int j = i + 1; j < n; j++)
        {
            if(priority[i] > priority[j])
            {
                swap(priority[i], priority[j]);
                swap(bt[i], bt[j]);
                swap(pid[i], pid[j]);
            }
        }
    }

    ct[0] = bt[0];

    for(int i = 1; i < n; i++)
    {
        ct[i] = ct[i - 1] + bt[i];
    }

    for(int i = 0; i < n; i++)
    {
        tat[i] = ct[i];
    }

    for(int i = 0; i < n; i++)
    {
        wt[i] = tat[i] - bt[i];
    }

    cout << "\nPID\tBT\tPriority\tCT\tTAT\tWT\n";

    for(int i = 0; i < n; i++)
    {
        cout << "P" << pid[i] << "\t"
             << bt[i] << "\t"
             << priority[i] << "\t\t"
             << ct[i] << "\t"
             << tat[i] << "\t"
             << wt[i] << endl;
    }

    float avg_wt = 0;
    float avg_tat = 0;

    for(int i = 0; i < n; i++)
    {
        avg_wt += wt[i];
        avg_tat += tat[i];
    }

    avg_wt = avg_wt / n;
    avg_tat = avg_tat / n;

    cout << "\nAverage Waiting Time = " << avg_wt << endl;
    cout << "Average Turnaround Time = " << avg_tat << endl;

    return 0;
}
