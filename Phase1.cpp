#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class OS
{
private:
    char M[100][4];
    char IR[4];
    char R[4];
    int IC;
    int SI;
    bool C;

    ifstream fin;
    ofstream fout;
    string line;

public:
    void init()
    {
        for (int i = 0; i < 100; i++)
            for (int j = 0; j < 4; j++)
                M[i][j] = ' ';

        IC = 0;
        SI = 0;
        C = false;

        for (int i = 0; i < 4; i++)
        {
            IR[i] = ' ';
            R[i] = ' ';
        }
    }

    int getAddr()
    {
        return (IR[2] - '0') * 10 + (IR[3] - '0');
    }

    void MOS()
    {
        int addr = getAddr();

        if (SI == 1)
        {
            getline(fin, line);
            int k = 0;

            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (k < line.size())
                    {
                        M[addr][j] = line[k];
                        k++;
                    }
                    else
                        M[addr][j] = ' ';
                }
                addr++;
            }
        }

        else if (SI == 2)
        {
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 4; j++)
                    fout << M[addr][j];
                addr++;
            }
            fout << endl;
        }

        else if (SI == 3)
        {
            fout << "\n------------------------------------------------------------------ \n";
        }

        SI = 0;
    }

    void EXECUTE()
    {
        while (true)
        {

            for (int i = 0; i < 4; i++)
            {
                IR[i] = M[IC][i];
            }

            IC++;

            if (IR[0] == 'G' && IR[1] == 'D')
            {
                SI = 1;
                MOS();
            }

            else if (IR[0] == 'P' && IR[1] == 'D')
            {
                SI = 2;
                MOS();
            }

            else if (IR[0] == 'H')
            {
                SI = 3;
                MOS();
                break;
            }

            else if (IR[0] == 'L' && IR[1] == 'R')
            {
                int addr = getAddr();
                for (int i = 0; i < 4; i++)
                    R[i] = M[addr][i];
            }

            else if (IR[0] == 'S' && IR[1] == 'R')
            {
                int addr = getAddr();
                for (int i = 0; i < 4; i++)
                    M[addr][i] = R[i];
            }

            else if (IR[0] == 'C' && IR[1] == 'R')
            {
                int addr = getAddr();
                C = true;
                for (int i = 0; i < 4; i++)
                    if (R[i] != M[addr][i])
                        C = false;
            }

            else if (IR[0] == 'B' && IR[1] == 'T')
            {
                int addr = getAddr();
                if (C == true)
                    IC = addr;
            }
        }
    }

    void LOAD()
    {
        fin.open("input.txt");
        fout.open("output.txt");

        if (!fin)
        {
            cout << "Error opening input file\n";
            return;
        }

        int mem_ptr = 0;

        while (getline(fin, line))
        {

            if (line.find("$AMJ") < line.size())
            {
                init();
                mem_ptr = 0;
            }

            else if (line.find("$DTA") < line.size())
            {
                EXECUTE();
            }

            else if (line.find("$END") < line.size())
            {
                printMemory();
            }

            else
            {
                int k = 0;
                while (k < line.size())
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (k < line.size())
                            M[mem_ptr][j] = line[k++];
                        else
                            M[mem_ptr][j] = ' ';
                    }
                    mem_ptr++;
                }
            }
        }

        fin.close();
        fout.close();
    }

    void printMemory()
    {
        cout << "\nMEMORY REPRESENTATION\n";
        for (int i = 0; i < 100; i++)
        {
            cout << "M[" << i << "] : ";
            for (int j = 0; j < 4; j++)
                cout << M[i][j];
            cout << endl;
        }
    }
};

int main()
{
    OS os;
    os.LOAD();
    cout << "Execution Complete. Check output.txt\n";
    return 0;
}
