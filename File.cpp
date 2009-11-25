#include "File.h"
#include "stdlib.h"

int const File::gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */
int const File::bzip2_magic[2] = {'B', 'Z'}; /* bzip2 magic header */

File::File(){
    fileType = UNKNOWN;
}

int File::Open(const String& fileName, const char* mode) {
    if (fileType != UNKNOWN) { Close();}

    if (strchr(mode, 'r')!=NULL) { // In reading mode
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
                    (fileHandle.bz2 = BZ2_bzopen(fileName.c_str(), mode))!=NULL) {
            fileType = BZIP2;
        } else if ((fileHandle.fp = fopen(fileName.c_str(), mode)) != NULL) {
            fileType = TEXT;
        } else {
            return -1;
        }
    } else { // In writing mode
        if (fileName.Length() > 3 && fileName[-3]=='.' 
            && ( fileName[-2]=='g' || fileName[-2]=='G')
            && ( fileName[-1]=='z' || fileName[-1]=='Z')
            && (fileHandle.gz = gzopen(fileName.c_str(), mode)) != NULL){
            fileType = GZIP;
        } else if (fileName.Length()>4 && fileName[-4]=='.' 
                   && ( fileName[-3]=='b' || fileName[-3]=='B' )
                   && ( fileName[-2]=='z' || fileName[-2]=='Z' )
                   && fileName[-1]=='2' 
                   && (fileHandle.gz = BZ2_bzopen(fileName.c_str(), mode)) != NULL){
            fileType = BZIP2;
        } else if ((fileHandle.fp = fopen(fileName.c_str(), mode)) != NULL) {
            fileType = TEXT;
        } else {
            return -1;
        }
    }
    printf("fileType = %d\n", fileType);
    return 0;
}

// read up to given length
// ret value: 0: file end

int File::Read(char* buf, unsigned int len) {
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
int File::Write(char* buf){
    return (this->Write(buf, strlen(buf)));
}
int File::WriteLine(char* buf){
    return (this->WriteLine(buf, strlen(buf)));
}
int File::Write(String& str) { 
    return (this->Write(str.GetBuf(), str.Length()));
}
int File::WriteLine(String& str) {
    return (this->WriteLine(str.GetBuf(), str.Length()));
}
int File::Write(char* buf, unsigned int len){
    int n = -1;
    switch(fileType) {
    case GZIP:
        n=gzwrite(fileHandle.gz, buf, len);
        break;
    case TEXT:
        n=fwrite(buf, sizeof(char), len, fileHandle.fp);
        break;
    case BZIP2:
        static int bzerror;
        n = BZ2_bzwrite(fileHandle.bz2, buf, sizeof(char) * len);
        break;
    default:
        printf("Should not be here!\n");
        break;
    }
    return n;
}
int File::WriteLine(char* buf, unsigned int len){
    int n;
    n = this->Write(buf, len);
    n += this->Write((char*)"\n", 1);
    return n;
}

void File::Close() {
    switch(fileType) {
    case GZIP:
        gzclose(fileHandle.gz);
        break;
    case TEXT:
        fclose(fileHandle.fp);
        break;
    case BZIP2:
        BZ2_bzclose(fileHandle.bz2);
        break;
    default:
        printf("Should not be here!\n");
        break;
    }
}

////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////
FileWriter::FileWriter(){
    buf = 0;
    buf = (char*) malloc ( sizeof(char)* DEFAULT_BUFFER_BLOCK_SIZE);
    writePos = 0;
    bufLen = 0;
    bufferHasContent = false;
}

FileWriter::~FileWriter(){
    if (buf) free(buf);
}

int FileWriter::Write(String& str){
    int len = str.Length();
    int nWritten = 0;
    int nTotalWritten = 0;
    int strOffset = 0;
    while (len+1 > bufLen - writePos) {
        nWritten = ((File*)this)->Write(str.GetBuf()+strOffset, bufLen - writePos);
        writePos = 0;
        strOffset += nWritten;
        len -= nWritten;
    }
    // move the rest into buffer
    memcpy(buf+writePos, str.GetBuf()+strOffset, (len));
    writePos += len;

    return nTotalWritten;
}

int FileWriter::WriteLine(String& str){
    
}

void FileWriter::Flush(){
    
}

