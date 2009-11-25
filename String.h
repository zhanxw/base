#ifndef _STRING_H_
#define _STRING_H_

#include <string.h>
#include "zlib.h"
#include "bzlib.h"

class File;
class String
{
private:
    const static unsigned int DEFAULT_SIZE=sizeof(int)/sizeof(char);
private:
    char* buf;
    unsigned int len;
    unsigned int cap;
public:
    String();
    String(const char* s);
    ~String();
public:
    const char* c_str() const { return ((const char*) buf); };
    String& Append(char c);
    String& Append(char* str);
    String& Append(char* str, int strLen);
    String& Strip();
    int Length() { return len;};
    void Clear();
public:
    String& Copy(char* str);
    String& Copy(char* str, int strLen);
    char& operator[] (unsigned int i);
};

char& String::operator [] (unsigned int i){
    return (buf[i]);
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

class File
{
private:
    union FileHandle_t {gzFile gz; FILE* fp; BZFILE* bz2; } fileHandle;
    static int const gz_magic[2];// = {0x1f, 0x8b}; /* gzip magic header */
    static int const bzip2_magic[2];// = {'B', 'Z'}; /* bzip2 magic header */
public:
    enum FileType{UNKNOWN, TEXT, GZIP, BZIP2} fileType;
public:
    File();

public:
    int Open(const String& fileName, const char* mode);
    int Read(char* buf, unsigned int len);
    int ReadLine(String& s);
    void Close();
};

int const File::gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */
int const File::bzip2_magic[2] = {'B', 'Z'}; /* bzip2 magic header */

File::File(){
    fileType = UNKNOWN;
}

int File::Open(const String& fileName, const char* mode) {
    if (fileType != UNKNOWN) { Close();}

    // detect file type by peeking first two bytes
    FILE * fp = fopen(fileName.c_str(), "rb");
    unsigned char header[2]={0,0};
    if (!fp) return -1;
    fread(header, sizeof(char), 2, fp);
    fclose(fp);
    
    if ( header[0] == gz_magic[0] && header[1] == gz_magic[1] &&
         (fileHandle.gz = gzopen(fileName.c_str(), mode)) != NULL){
        fileType = GZIP;
    } else if ( header[0] == bzip2_magic[0] && header[1] == bzip2_magic[1] &&
                (fileHandle.bz2 = BZ2_bzopen(fileName.c_str(), "r"))!=NULL) {
        fileType = BZIP2;
    } else if ((fileHandle.fp = fopen(fileName.c_str(), mode)) != NULL) {
        fileType = TEXT;
    } else {
        return -1;
    }
    printf("fileType = %d\n", fileType);
    return 0;
}

// read up to given length
// ret value: 0: file end

int File::Read(char* buf, unsigned int len) {
//    printf("fileType = %d\n", fileType);
    int n=-1;
    switch(fileType) {
    case GZIP:
        n=gzread(fileHandle.gz, buf, len);
        break;
    case TEXT:
        n=fread(buf, sizeof(char), len, fileHandle.fp);
        break;
    case BZIP2:
        static int bzerror;
        n = BZ2_bzRead(&bzerror, fileHandle.bz2, buf, sizeof(char)*len);
        break;
    default:
        printf("Should not be here!\n");
        break;
    }
    return n;
}
int File::ReadLine(String& s){
    s.Clear();
    static bool lastCharIsNewLine = false;
    int i;
    char c;
    int n=0;
    bool firstChar = true;
    switch(fileType) {
    case GZIP:
        if (gzeof(fileHandle.gz))
            return -1;
        while ( (i = gzgetc(fileHandle.gz))>0){
            c = (char)(i);
            if (c == '\r')
                continue;
            s.Append(c);
            n++;
            if (c =='\n')
                break;
        }
        return n;
        break;
    case TEXT:
        if (feof(fileHandle.fp))
            return -1;
        while ( (i = fgetc(fileHandle.fp))>0){
            c = (char)(i);
            s.Append(c);
            if (c == '\r')
                continue;
            s.Append(c);
            n++;
            if (c =='\n')
                break;
        }
        return n;
        break;
    case BZIP2:
        static int bzerror;
        while ( (i = BZ2_bzRead(&bzerror, fileHandle.bz2, &c, sizeof(char))) > 0){
            if (c == '\r')
                continue;
            s.Append(c);
            n++;
            if (c =='\n')
                break;
        }
        if (bzerror != BZ_OK && i==0)
            return -1;
        return n;
        break;
    default:
        printf("Should not be here!\n");
        return -1;
        break;
    }
    return n;
}
void File::Close() {
    switch(fileType) {
    case GZIP:
        gzclose(fileHandle.gz);
        break;
    case BZIP2:
        BZ2_bzclose(fileHandle.bz2);
        break;
    case TEXT:
        fclose(fileHandle.fp);
        break;
    }
    fileType = UNKNOWN;
    return;
}

// default file system block size
#define DEFAULT_BUFFER_BLOCK_SIZE 4096
class FileReader:public File
{
private:
    char* buf;
    int readPos;
    int bufLen;
    bool bufferHasContent;
public:
    FileReader();
    ~FileReader();
public:
    int ReadLine(String& s);
};

FileReader::FileReader() {
    buf = 0;
    buf = (char*) malloc ( sizeof(char)* DEFAULT_BUFFER_BLOCK_SIZE);
    readPos = 0;
    bufLen = 0;
    bufferHasContent = false;
}

FileReader::~FileReader() {
    if (!buf) free(buf);
}

// return value:
// -1: end of file
// 0: empty line
// 1,2,.. : read n characters.
int FileReader::ReadLine(String& s) {
    s.Clear();
    while (true) {
        if (!bufferHasContent) {
            bufLen = Read(buf, DEFAULT_BUFFER_BLOCK_SIZE);
            
            if (bufLen == 0) { // end of file
                s.Clear();
                return -1;
            } else {
                bufferHasContent = true;
                readPos = 0;
            }
        }
        // find a line beginning
        int lineStart = readPos;
        while (lineStart < bufLen && (buf[lineStart]=='\r'))
            lineStart++;
        if (lineStart == bufLen){ // nothing to read
            s.Clear();
            return 0;
        }

        int lineEnd = lineStart;
        while(lineEnd < bufLen && buf[lineEnd]!='\n' && buf[lineEnd]!='\r')
            lineEnd++;
        
        if (lineEnd == bufLen) { // need to read whole buffer
            s.Append(&buf[lineStart], bufLen-lineStart);
            bufferHasContent = false;
        } else { // read (lineStart - lineEnd) buffer
            s.Append(&buf[lineStart], lineEnd-lineStart);
            readPos = lineEnd+1;
            if (readPos >= bufLen)
                bufferHasContent = false;
            return (lineEnd-lineStart);
        }
    }
    return -1;
}

class FileWriter: public File
{
    FileWriter();
    ~FileWriter();
    void Flush();
};

FileWriter::FileWriter(){
}

FileWriter::~FileWriter(){
}

void FileWriter::Flush(){
}
#endif /* _STRING_H_ */
