#include "engine.h"

#define LOGSTRLEN 512


namespace study
{

int participant_id = NULL;
FILE* eventlogfile = NULL;
FILE* summarylogfile = NULL;
game_round* this_round = new game_round;
vector<game_round>* rounds = new vector<game_round>;

void set_participant_id(int id)
{
    participant_id = id;
}

void reset_round_struct()
{
    delete this_round;
    this_round = new game_round;
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
    if(!participant_id)
    {
        logoutf("No participant ID given. Log failed.");
        return;
    }
    // write summary log file with all values but the events
    string filename;
    formatstring(filename, "logs/p%i_r%i_sum.txt", participant_id, this_round->round_number);
    logoutf("%i", this_round->round_number);
    logoutf(filename);
    setsummarylogfile(filename);
    summarylogoutf("TCS:%li", this_round->completion_time);
    summarylogoutf("deaths:%i", this_round->deaths);
    summarylogoutf("kills:%i", this_round->kills);
    summarylogoutf("delay_min:%i", this_round->delay_min);
    summarylogoutf("delay_max:%i", this_round->delay_max);

    // log events to csv
    formatstring(filename, "logs/p%i_r%i_events.csv", participant_id, this_round->round_number);
    seteventlogfile(filename);
    // write header
    eventlogoutf(
        "timestamp;event_name;input_type;input_value;delay;shot_hit"
    );
    for(int i=0; i<this_round->events.length(); ++i)
    {
        round_event ev = this_round->events[i];
        eventlogoutf(
            "%li;%s;%s;%s;%i;%i",
            ev.timestamp,
            ev.event_name,
            ev.input_type,
            ev.input_value,
            ev.delay_this,
            ev.shot_hit
        );
    }
}

void closeeventlogfile()
{
    if(eventlogfile)
    {
        fclose(eventlogfile);
        eventlogfile = NULL;
    }
}

void closesummarylogfile()
{
    if(summarylogfile)
    {
        fclose(summarylogfile);
        summarylogfile = NULL;
    }
}

uint64_t epoch_time_ms()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long int ms = time.tv_sec * 1000 + time.tv_usec / 1000;
    return ms;
}

FILE* geteventlogfile()
{
    return eventlogfile;
}

void seteventlogfile(const char *fname)
{
    closeeventlogfile();
    if(fname && fname[0])
    {
        fname = findfile(fname, "w");
        if(fname) eventlogfile = fopen(fname, "w");
    }
    FILE *f = geteventlogfile();
    if(f) setvbuf(f, NULL, _IOLBF, BUFSIZ);
}

FILE* getsummarylogfile()
{
    return summarylogfile;
}

void setsummarylogfile(const char *fname)
{
    closesummarylogfile();
    if(fname && fname[0])
    {
        fname = findfile(fname, "w");
        if(fname) summarylogfile = fopen(fname, "w");
    }
    FILE *f = getsummarylogfile();
    if(f) setvbuf(f, NULL, _IOLBF, BUFSIZ);
}

void summarylogoutf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    summarylogoutfv(fmt, args);
    va_end(args);
}

void summarylogoutfv(const char *fmt, va_list args)
{
    FILE *f = getsummarylogfile();
    if(f) writelogv(f, fmt, args);
}

void eventlogoutf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    eventlogoutfv(fmt, args);
    va_end(args);
}

void eventlogoutfv(const char *fmt, va_list args)
{
    FILE *f = geteventlogfile();
    if(f) writelogv(f, fmt, args);
}

}