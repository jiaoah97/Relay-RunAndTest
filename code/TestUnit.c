#include <stdio.h>
#include <time.h>
#define ROWSIZE 24000
#define COLSIZE 11

double rawdata[ROWSIZE][COLSIZE];
double timeData[ROWSIZE];
int onOff[ROWSIZE];
double port1[ROWSIZE][9];
double port2[ROWSIZE][9];
int tripSignal[ROWSIZE][3];

extern void s1_line1_(double* time, int* onOff, double* port1, double* port2, int* tripSignal);

void readData(char* filename, double data[][COLSIZE], int row, int col) {
    FILE *fw = fopen(filename, "r");
    int i, j;

    if (filename == NULL) return;

    if (fw != NULL)
    {
        for (i = 0; i < row; i++)
        {
            for (j = 0; j < col; j++)
            {
                fscanf(fw, "%lf", &data[i][j]); //读取文件中的数据，遇到空格和换行停止读。
            }
        }
        fclose(fw);
    }
}


int main()
{
    int i;

    readData("../500kV_Model.gf42/user_01.out", rawdata, ROWSIZE, COLSIZE);

    for (i = 0; i < ROWSIZE; i++) {
        // 时间
        timeData[i] = rawdata[i][0];

        // 电压
        port1[i][0] = rawdata[i][4];
        port1[i][1] = rawdata[i][5];
        port1[i][2] = rawdata[i][6];

        // 电流
        port1[i][3] = rawdata[i][1];
        port1[i][4] = rawdata[i][2];
        port1[i][5] = rawdata[i][3];

        // 对侧电量
        port2[i][0] = 0;
        port2[i][1] = 0;
        port2[i][2] = 0;
        port2[i][3] = 0;
        port2[i][4] = 0;
        port2[i][5] = 0;


        tripSignal[i][0] = 0;
        tripSignal[i][1] = 0;
        tripSignal[i][2] = 0;

        onOff[i] = 1;
    }

    for (i = 0; i < ROWSIZE; i++) {
        s1_line1_(&timeData[i], &onOff[i], port1[i], port2[i], tripSignal[i]);
    }
}
