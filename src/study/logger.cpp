#include "engine.h"

#define LOGSTRLEN 512

namespace study
{

FILE *studylogfile = NULL;
game_round* this_round = new game_round;
vector<game_round>* rounds = new vector<game_round>;

void reset_round_struct()
{
    delete this_round;
    this_round = new game_round;
}

void init()
{
    // this_round = new game_round;
    // rounds = new vector<game_round>;
}

game_round* get_this_round()
{
    return this_round;
}

vector<game_round>* get_all_rounds()
{
    return rounds;
}


void write_to_file()
{

}

void closestudylogfile()
{
    if(studylogfile)
    {
        fclose(studylogfile);
        studylogfile = NULL;
    }
}

uint64_t epoch_time_ms()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long int ms = time.tv_sec * 1000 + time.tv_usec / 1000;
    return ms;
}

FILE* getstudylogfile()
{
    return studylogfile;
}

void setstudylogfile(const char *fname)
{
    closestudylogfile();
    if(fname && fname[0])
    {
        fname = findfile(fname, "w");
        if(fname) studylogfile = fopen(fname, "w");
    }
    FILE *f = getstudylogfile();
    if(f) setvbuf(f, NULL, _IOLBF, BUFSIZ);
}

void studylogoutf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    studylogoutfv(fmt, args);
    va_end(args);
}

void studylogoutfv(const char *fmt, va_list args)
{
    FILE *f = getstudylogfile();
    // if(f) writelogv(f, fmt, args);
}

// void writelog(FILE *file, const char *buf)
// {
//     static uchar ubuf[512];
//     size_t len = strlen(buf), carry = 0;
//     while(carry < len)
//     {
//         size_t numu = encodeutf8(ubuf, sizeof(ubuf)-1, &((const uchar *)buf)[carry], len - carry, &carry);
//         if(carry >= len) ubuf[numu++] = '\n';
//         fwrite(ubuf, 1, numu, file);
//     }
// }

// void writelogv(FILE *file, const char *fmt, va_list args)
// {
//     static char buf[LOGSTRLEN];
//     vformatstring(buf, fmt, args, sizeof(buf));
//     writelog(file, buf);
// }

}