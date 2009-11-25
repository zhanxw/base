#ifndef _FILE_H_
#define _FILE_H_

#include "String.h"

class String;
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
    int Write(char* buf);
    int WriteLine(char* buf);
    int Write(String& str);
    int WriteLine(String& str);
    int Write(char* str, unsigned int len);
    int WriteLine(char* str, unsigned int len);
    void Close();
};

////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////

class FileWriter: public File
{
private:
    char* buf;
    int writePos;
    int bufLen;
    bool bufferHasContent;
public:
    FileWriter();
    ~FileWriter();
    int Write(String& str);
    int WriteLine(String& str);
    void Flush();
};


#endif /* _FILE_H_ */
