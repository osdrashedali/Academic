#include<bits/stdc++.h>
using namespace std;
int main(){
    
    int n;
    cout<<"Enter number of processes : ";
    cin>>n;
    cout<<endl;
     int p[n],bt[n],wt[n],tat[n];
    for(int i=0 ; i<n ; i++){
        p[i]=i+1;
        cout<<"Enter Brust time of P"<<p[i]<<" : ";
        cin>>bt[i];
    }

    for(int i=0 ; i<n-1 ;i++){
        for(int j=i+1;j<n;j++){
            if(bt[i]>bt[j]){
                swap(bt[i],bt[j]);
                swap(p[i],p[j]);
            }
        }
    }
    wt[0]=0;
    tat[0]=wt[0] + bt[0];
    for(int i=1; i<n ;i++){
        wt[i] = wt[i-1] + bt[i-1];
        tat[i]= wt[i] + bt[i];
    }
    
    double total =0;
    double total_tat = 0;
    for(int i=0; i<n ; i++){
        
        cout<<"\nP"<<p[i]<<" -> "<<"Brust time : "<<bt[i]<<"\t"<<"Waiting time :"<<wt[i]<<"\t\t"<<"TAT : "<<tat[i];
        total += wt[i];
        total_tat += tat[i];
        cout << endl;
        
    }
    cout <<"\nAVg waiting time : "<<total/n<<endl;
    cout <<"\nAVg Turn around time : "<<total_tat/n<<endl;
    

    return 0;
}
