#include <bits/stdc++.h>
using namespace std;

class Process
{
public:
    string tag;        // Nombre del proceso
    int AT, BT, Q, Pr; // Arrival Time, Burst Time, Queue, Priority

    // Métricas
    int CT, WT, RT, TAT;
    bool firstRun;
    int remaining;
    int lastExec; // última vez que estuvo en CPU (para WT)

    Process(string t, int at, int bt, int q, int pr)
        : tag(t), AT(at), BT(bt), Q(q), Pr(pr),
          CT(0), WT(0), RT(0), TAT(0),
          firstRun(true), remaining(bt), lastExec(at) {}
};

// ---- Algoritmo MLFQ ----
void mlfq(vector<Process> &processes, vector<int> quantums)
{
    int time = 0;
    int n = processes.size();
    int completed = 0;

    // Orden lexicográfico si AT es igual
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         {
        if (a.AT == b.AT) return a.tag < b.tag;
        return a.AT < b.AT; });

    // Recorremos cada quantum en secuencia
    int level = 0;
    while (completed < n)
    {
        bool avanzamos = false;

        for (int i = 0; i < n; i++)
        {
            Process &p = processes[i];
            if (p.remaining <= 0)
                continue; // ya terminado, saltar

            avanzamos = true;
            // Si es la primera vez que se ejecuta, calculamos RT
            if (p.firstRun)
            {
                if (time < p.AT)
                    time = p.AT; // esperar si aún no llegó
                p.RT = time - p.AT;
                p.WT = p.RT;
                p.firstRun = false;
            }
            else
            {
                // acumula el tiempo de espera
                p.WT += (time - p.lastExec);
            }

            int exec = min(quantums[level], p.remaining);
            time += exec;
            p.remaining -= exec;
            p.lastExec = time;

            if (p.remaining == 0)
            {
                p.CT = time;
                p.TAT = p.CT - p.AT;
                completed++;
            }
        }

        // Avanzamos al siguiente nivel de quantum, pero si ya estamos en el último, nos quedamos allí
        if (level < (int)quantums.size() - 1)
            level++;
        else if (!avanzamos)
            break; // no hay más que ejecutar
    }
}

// ---- Print results ----
void printResults(const vector<Process> &processes)
{
    cout << "# RR(3), RR(5), RR(6), RR(20).\n";
    cout << "# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n";

    double avgCT = 0, avgWT = 0, avgRT = 0, avgTAT = 0;

    for (auto &p : processes)
    {
        cout << p.tag << ";"
             << p.BT << "; "
             << p.AT << "; "
             << p.Q << "; "
             << p.Pr << "; "
             << p.WT << "; "
             << p.CT << "; "
             << p.RT << "; "
             << p.TAT << "\n";

        avgCT += p.CT;
        avgWT += p.WT;
        avgRT += p.RT;
        avgTAT += p.TAT;
    }

    int n = processes.size();
    cout << "WT=" << fixed << setprecision(1) << avgWT / n
         << "; CT=" << avgCT / n
         << "; RT=" << avgRT / n
         << "; TAT=" << avgTAT / n << ";\n";
}

// ---- Main ----
int main()
{
    vector<Process> processes;
    string line;

    // Leer input tipo: A;40;0;1;5
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

    // Definir niveles de quantum para la MLFQ
    vector<int> quantums = {3, 5, 6, 20};
    mlfq(processes, quantums);
    printResults(processes);

    return 0;
}
