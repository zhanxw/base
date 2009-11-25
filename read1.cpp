#include "StringArray.h"

int main() 
{
    String fn = "/home/zhanxw/compareMapSoft/index/mapreads/chr1.fa";
    IFILE file = ifopen(fn.c_str(), "r");

    int totalChar = 0;
    String line;
    int freq[256] = {0};
    while (!ifeof(file)){
        line.ReadLine(file);
        totalChar += line.Length();
        for (int i = 0; i < line.Length(); i++)
            freq[(unsigned int) line[i]]++;
    }
    printf("A frequency: %d (%f)\n", freq[(int)'A'], (float)freq[(int)'A']/totalChar);
    printf("T frequency: %d (%f)\n", freq[(int)'T'], (float)freq[(int)'T']/totalChar);
    printf("G frequency: %d (%f)\n", freq[(int)'G'], (float)freq[(int)'G']/totalChar);
    printf("C frequency: %d (%f)\n", freq[(int)'C'], (float)freq[(int)'C']/totalChar);
}
