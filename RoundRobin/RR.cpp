#include <bits/stdc++.h>
using namespace std;

class Process
{
public:
    string tag;        // tag
    int AT, BT, Q, Pr; // Arrival Time, Burst Time, Queue, Priority

    // Métricas
    int CT, WT, RT, TAT; // Completion Time, Waiting Time, Response Time, Turn Around Time
    bool firstRun;       // Para RT
    int remaining;       // Tiempo restante por ejecutar

    Process(string t, int at, int bt, int q, int pr)
        : tag(t), AT(at), BT(bt), Q(q), Pr(pr),
          CT(0), WT(0), RT(0), TAT(0),
          firstRun(true), remaining(bt) {}
};

// ---- Algoritmo Round Robin ----
void roundRobin(vector<Process> &processes, int quantum)
{
    int time = 0, completed = 0;
    int n = processes.size();

    queue<int> ready;               // Cola de procesos listos
    vector<bool> inQueue(n, false); // Saber si ya están en la cola
    vector<int> lastExec(n, 0);     // Última vez que corrieron

    // Ordenar primero por AT, luego lexicográficamente por tag
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         {
        if (a.AT == b.AT) return a.tag < b.tag;
        return a.AT < b.AT; });

    // Insertar procesos iniciales que llegan en t=0
    for (int i = 0; i < n; i++)
    {
        if (processes[i].AT == 0)
        {
            ready.push(i);
            inQueue[i] = true;
        }
    }

    // Bucle principal hasta terminar todos los procesos
    while (completed < n)
    {
        if (ready.empty())
        {
            // Si la cola está vacía, saltar al siguiente proceso
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++)
                if (processes[i].remaining > 0)
                    nextArrival = min(nextArrival, processes[i].AT);

            time = max(time, nextArrival);
            for (int i = 0; i < n; i++)
            {
                if (!inQueue[i] && processes[i].AT <= time && processes[i].remaining > 0)
                {
                    ready.push(i);
                    inQueue[i] = true;
                }
            }
            continue;
        }

        int idx = ready.front();
        ready.pop(); // Tomar proceso de la cola
        Process &p = processes[idx];

        if (p.firstRun)
        {
            p.RT = time - p.AT; // RT = primera vez que entra a CPU - AT
            p.WT = p.RT;        // Inicialmente WT = RT (primer espera)
            p.firstRun = false;
        }
        else
        {
            // Acumular el tiempo que esperó desde la última ejecución
            p.WT += time - lastExec[idx];
        }

        // Ejecutar por quantum o por el tiempo que le falte
        int exec = min(quantum, p.remaining);
        time += exec;
        p.remaining -= exec;
        lastExec[idx] = time; // Actualizar última ejecución

        // Agregar procesos que llegaron mientras este corría
        for (int i = 0; i < n; i++)
        {
            if (!inQueue[i] && processes[i].AT <= time && processes[i].remaining > 0)
            {
                ready.push(i);
                inQueue[i] = true;
            }
        }

        if (p.remaining > 0)
        {
            ready.push(idx); // Si le queda trabajo, vuelve a la cola
        }
        else
        {
            p.CT = time;         // Tiempo de finalización
            p.TAT = p.CT - p.AT; // TAT = CT - AT
            completed++;
        }
    }
}

// ---- Imprimir resultados ----
void printResults(const vector<Process> &processes)
{
    cout << "PID\tAT\tBT\tCT\tTAT\tWT\tRT\n";

    double avgCT = 0, avgTAT = 0, avgWT = 0, avgRT = 0;

    for (auto &p : processes)
    {
        cout << p.tag << "\t" << p.AT << "\t" << p.BT << "\t"
             << p.CT << "\t" << p.TAT << "\t" << p.WT << "\t" << p.RT << "\n";

        // Acumular para promedios
        avgCT += p.CT;
        avgTAT += p.TAT;
        avgWT += p.WT;
        avgRT += p.RT;
    }

    int n = processes.size();
    cout << "-----------------------------------------\n";
    cout << "AVG\t-\t-\t"
         << fixed << setprecision(2)
         << avgCT / n << "\t" << avgTAT / n << "\t" << avgWT / n << "\t" << avgRT / n << "\n";
}

// ---- Main ----
int main()
{
    vector<Process> processes;
    string line;

    // Leer input tipo: A;6;0;1;5
    while (getline(cin, line))
    {
        if (line.empty() || line[0] == '#')
            continue;
        stringstream ss(line);
        string seg;

        getline(ss, seg, ';');
        string tag = seg;
        getline(ss, seg, ';');
        int bt = stoi(seg);
        getline(ss, seg, ';');
        int at = stoi(seg);
        getline(ss, seg, ';');
        int q = stoi(seg);
        getline(ss, seg, ';');
        int pr = stoi(seg);

        Process p(tag, at, bt, q, pr);
        processes.push_back(p);
    }

    int quantum = 5;
    roundRobin(processes, quantum);
    printResults(processes);

    return 0;
}
