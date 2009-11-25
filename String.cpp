#include "String.h"
#include "stdlib.h"

char& String::operator [] (int i){
    return (i>=0 ? buf[i]: buf[len+i] );
}

char& String::operator [] (int i) const{
    return (i>=0 ? buf[i]: buf[len+i] );
}

String::String() {
    cap = DEFAULT_SIZE;
    buf = (char*) malloc (sizeof(char)*DEFAULT_SIZE);
    buf[0] = '\0';
    len = 0;
}

String::String(const char* s) {
    if (!s) return;
    char* p = (char*) s;
    len = 0;
    while (*p++ != '\0') len++;
    buf = (char*) malloc (sizeof(char)*(len+1));
    memcpy(buf, s, len);
    buf[len]='\0';
    cap = len+1;
}

String::~String() {
    if(buf) free(buf);
}

String& String::Copy(char* str, int strLen){
    while (cap < strLen+1 ) {
        cap += DEFAULT_SIZE;
        buf = (char*) realloc(buf, sizeof(char)*cap);
    }
    memcpy(buf, str, strLen);
    buf[strLen]='\0';

    return *this;
}

String& String::Copy(char* str){
    int strLen = strlen(str);
    return this->Copy(str, strLen);
}

void String::Clear() {
    if (buf) buf[0]= '\0';
    len = 0;
}

String& String::Append(char c){
    if (len+1 < cap){
        buf[len++]=c;
        buf[len]='\0';
    } else {
        cap += sizeof(int*) / sizeof(char); // 64/8 = 8 on 64 bit computer.
        buf = (char*) realloc(buf, sizeof(char)*(cap));
        buf[len++]=c;
        buf[len]='\0';
    }
    return *this;
}

String& String::Append(char* str){
    int strLen = strlen(str);
    return this->Append(str, strLen);
}

String& String::Append(char* str, int strLen){
    bool reallocMemory = false;
    if (len+1 + strLen > cap)
        reallocMemory = true;
    while (len+1 + strLen > cap)
        cap += sizeof(int*) / sizeof(char); // 64/8 = 8 on 64 bit computer.
    buf = (char*) realloc(buf, sizeof(char)*(cap));
    memcpy(buf+len, str, strLen);
    len +=strLen;
    buf[len]='\0';

    return *this;
}

String& String::Strip(){
    if (len>0) {
        const char SMALL = '!'; // lowest visible character in ASCILL table
        const char BIG = '~';   // highest visible character in ASCILL table
        int start = 0;
        while (start < len && ( buf[start] < SMALL|| buf[start] > BIG)) start++;
        int end = len-1;
        while (end>0 && ( buf[end] < SMALL || buf[end] > BIG )) end--;
        len = end - start + 1;
        if (start > 0)
            memmove(buf, buf+start, len);
        buf[len]='\0';
    }
    return (*this);
}

class StringArray
{

};
