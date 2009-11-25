#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "String.h"
#include <stdint.h>

#define PD(x) printf("%d\n",(x));

int main()
{
#ifdef CODE1
    // test 1
    String fn = "/home/zhanxw/compareMapSoft/index/mapreads/chr1.fa";
    String line;
    int totalChar = 0;
    int freq[256] = {0};
    FileReader f;
    f.Open(fn, "rt");
    while (f.ReadLine(line)>=0) {
        line.Strip();
        totalChar += line.Length();
        for (int i = 0; i < line.Length(); i++)
            freq[(unsigned int) line[i]]++;
    }
    f.Close();
    printf("A frequency: %d (%f)\n", freq[(int)'A'], (float)freq[(int)'A']/totalChar);
    printf("T frequency: %d (%f)\n", freq[(int)'T'], (float)freq[(int)'T']/totalChar);
    printf("G frequency: %d (%f)\n", freq[(int)'G'], (float)freq[(int)'G']/totalChar);
    printf("C frequency: %d (%f)\n", freq[(int)'C'], (float)freq[(int)'C']/totalChar);
#endif

#ifndef CODE2
    //String fn("testz.gz");
    String fn("testj.bz2");
    String line;
//    printf("%s", (char*)line);
    char buf[1024];
    int len = 1024;
    //File f;
    FileReader f;
    f.Open(fn, "rb");
    while (f.ReadLine(line)>=0) {
        line.Strip();
        printf("%s\n", line.c_str());
    }
    f.Close();

    printf("----------------\n");
    printf("%s\n",fn.c_str());
    printf("end\n");
#endif

//     char bStr[]="bb";
//     func("aa");
//     func("bb");

//     PD(sizeof(char));
//     PD(sizeof(short));
//     PD(sizeof(int));
//     PD(sizeof(uint16_t));
//     PD(sizeof(uint32_t));
//     PD(sizeof(float));
//     PD(sizeof(int*));
//     PD(sizeof(double));
    return 0;
}
