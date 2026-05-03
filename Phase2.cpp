#include <bits/stdc++.h>
using namespace std;

char M[300][4];
char IR[4], R[4];
int IC;
bool C;
int SI, PI, TI;
int PTR;
int TTC, LLC;
int TTL, TLL;

ifstream fin;
ofstream fout;

vector<int> usedFrames;
int currentPage = 0;
bool terminated = false;

// ====================== INIT ======================
void init()
{
    for (int i = 0; i < 300; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            M[i][j] = ' ';
        }
    }

    for (int i = 0; i < 4; i++)
    {
        IR[i] = ' ';
        R[i] = ' ';
    }

    IC = 0;
    C = false;
    SI = PI = TI = 0;
    TTC = LLC = 0;
    currentPage = 0;
    terminated = false;
    usedFrames.clear();
}

// ====================== ALLOCATE FRAME ======================
int allocateFrame()
{
    if (usedFrames.size() >= 30)
    {
        terminate(6); // No more frames
        exit(1);
    }
    while (true)
    {
        int f = rand() % 30;
        if (find(usedFrames.begin(), usedFrames.end(), f) == usedFrames.end())
        {
            usedFrames.push_back(f);
            return f;
        }
    }
}

// ====================== ADDRESS MAP ======================
int addressMap(int VA)
{
    if (VA < 0 || VA > 99)
    {
        PI = 2;
        return -1;
    }
    int page = VA / 10;
    int PTE = PTR + page;

    if (M[PTE][0] == '*' || !isdigit(M[PTE][2]) || !isdigit(M[PTE][3]))
    {
        PI = 3;
        return -1;
    }
    int frame = (M[PTE][2] - '0') * 10 + (M[PTE][3] - '0');
    return frame * 10 + (VA % 10);
}

// ====================== TERMINATE ======================
void terminate(int EM)
{
    fout << "\n\n";
    switch (EM)
    {
    case 0:
        fout << "NO ERROR\n";
        break;
    case 1:
        fout << "OUT OF DATA\n";
        break;
    case 2:
        fout << "LINE LIMIT EXCEEDED\n";
        break;
    case 3:
        fout << "TIME LIMIT EXCEEDED\n";
        break;
    case 4:
        fout << "OPERATION CODE ERROR\n";
        break;
    case 5:
        fout << "OPERAND ERROR\n";
        break;
    case 6:
        fout << "INVALID PAGE FAULT\n";
        break;
    }
    terminated = true;
}

// ====================== MASTER MODE ======================
void MOS()
{
    if (terminated)
        return;

    // TI has highest priority
    if (TI == 2)
    {
        if (SI == 2)
        { // Allow last PD
            LLC++;
            if (LLC <= TLL)
            {
                int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
                int RA = addressMap(VA);
                if (PI == 0)
                {
                    for (int i = RA; i < RA + 10; i++)
                        for (int j = 0; j < 4; j++)
                            fout << M[i][j];
                    fout << "\n";
                }
            }
        }
        terminate(3);
        return;
    }

    // PI Handling
    if (PI != 0)
    {
        if (PI == 3)
        {
            if ((IR[0] == 'G' && IR[1] == 'D') || (IR[0] == 'S' && IR[1] == 'R'))
            {
                int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
                int page = VA / 10;
                int frame = allocateFrame();
                M[PTR + page][0] = '0';
                M[PTR + page][1] = '0';
                M[PTR + page][2] = (frame / 10) + '0';
                M[PTR + page][3] = (frame % 10) + '0';
                PI = 0;
                return;
            }
            else
            {
                terminate(6);
                return;
            }
        }
        else if (PI == 1)
            terminate(4);
        else if (PI == 2)
            terminate(5);
        return;
    }

    // SI Handling
    if (SI == 1)
    { // GD
        string line;
        if (!getline(fin, line) || line.substr(0, 4) == "$END")
        {
            terminate(1);
            return;
        }
        int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
        int RA = addressMap(VA);
        if (PI != 0)
            return;

        int k = 0;
        for (int i = RA; i < RA + 10; i++)
            for (int j = 0; j < 4; j++)
                M[i][j] = (k < (int)line.length()) ? line[k++] : ' ';
        SI = 0;
    }
    else if (SI == 2)
    { // PD
        LLC++;
        if (LLC > TLL)
        {
            terminate(2);
            return;
        }
        int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
        int RA = addressMap(VA);
        if (PI != 0)
            return;

        for (int i = RA; i < RA + 10; i++)
            for (int j = 0; j < 4; j++)
                fout << M[i][j];
        fout << "\n";
        SI = 0;
    }
    else if (SI == 3)
    {
        terminate(0);
    }
}

// ====================== EXECUTE ======================
void executeUserProgram()
{
    while (!terminated)
    {
        if (TTC >= TTL)
        {
            TI = 2;
            MOS();
            continue;
        }

        int RA = addressMap(IC);
        if (PI != 0)
        {
            MOS();
            if (PI == 0)
            { // Page fault resolved
                IC--;
                continue;
            }
            return;
        }

        memcpy(IR, M[RA], 4);
        IC++;

        if (!isdigit(IR[2]) || !isdigit(IR[3]))
        {
            PI = 2;
            MOS();
            return;
        }

        string op(IR, IR + 2);

        if (op == "GD")
        {
            SI = 1;
            MOS();
        }
        else if (op == "PD")
        {
            SI = 2;
            MOS();
        }
        else if (op == "LR")
        {
            int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
            int loc = addressMap(VA);
            if (PI != 0)
            {
                MOS();
                continue;
            }
            memcpy(R, M[loc], 4);
        }
        else if (op == "SR")
        {
            int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
            int loc = addressMap(VA);
            if (PI != 0)
            {
                MOS();
                continue;
            }
            memcpy(M[loc], R, 4);
        }
        else if (op == "CR")
        {
            int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
            int loc = addressMap(VA);
            if (PI != 0)
            {
                MOS();
                continue;
            }
            C = (memcmp(R, M[loc], 4) == 0);
        }
        else if (op == "BT")
        {
            int VA = (IR[2] - '0') * 10 + (IR[3] - '0');
            if (C)
            {
                int oldPI = PI;
                int test = addressMap(VA);
                if (PI == 0)
                    IC = VA;
                PI = oldPI; // Restore PI
            }
        }
        else if (IR[0] == 'H')
        {
            SI = 3;
            MOS();
            return;
        }
        else
        {
            PI = 1;
            MOS();
            return;
        }
        TTC++;
    }
}

// ====================== LOAD ======================
void load()
{
    string line;
    while (getline(fin, line))
    {
        if (line.empty())
            continue;

        if (line.substr(0, 4) == "$AMJ")
        {
            init();
            TTL = stoi(line.substr(8, 4));
            TLL = stoi(line.substr(12, 4));
            PTR = allocateFrame() * 10;
            for (int i = PTR; i < PTR + 10; i++)
                M[i][0] = M[i][1] = M[i][2] = M[i][3] = '*';
        }
        else if (line.substr(0, 4) == "$DTA")
        {
            executeUserProgram();
        }
        else if (line.substr(0, 4) == "$END")
        {
            continue;
        }
        else
        {
            if (currentPage >= 10)
            {
                terminate(6);
                break;
            }
            int frame = allocateFrame();
            int page = currentPage++;

            M[PTR + page][0] = '0';
            M[PTR + page][1] = '0';
            M[PTR + page][2] = (frame / 10) + '0';
            M[PTR + page][3] = (frame % 10) + '0';

            int k = 0;
            for (int i = frame * 10; i < frame * 10 + 10; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    M[i][j] = (k < (int)line.length()) ? line[k++] : ' ';
                }
            }
        }
    }
}

int main()
{
    srand(time(0));
    fin.open("input.txt");
    fout.open("output.txt");

    if (!fin.is_open())
    {
        cout << "Error opening input.txt\n";
        return 1;
    }

    load();

    fin.close();
    fout.close();
    cout << "MOS Execution Completed!\n";
    return 0;
}
