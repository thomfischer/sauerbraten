#include "engine.h"
#include "game.h"

#define LOGSTRLEN 512


namespace study
{

FILE* eventlogfile = NULL;
FILE* summarylogfile = NULL;
game_round* this_round = new game_round;
vector<game_round>* rounds = new vector<game_round>;

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
    // overwrite possibly unset variables with -1; would be sizeof var otherwise
    if(!this_round->completion_time) this_round->completion_time = -1;
    if(!this_round->kills) this_round->kills = -1;
    if(!this_round->deaths) this_round->deaths = -1;
    if(!this_round->delay_max) this_round->delay_max = -1;
    if(!this_round->delay_min) this_round->delay_min = -1;

    // write summary log file with all values but the events
    string filename;
    formatstring(filename, "logs/%s_r%i_sum.txt", game::player1->name, this_round->round_number);
    logoutf("%i", this_round->round_number);
    logoutf(filename);
    setsummarylogfile(filename);
    summarylogoutf("TCS:%li", this_round->completion_time);
    summarylogoutf("deaths:%i", this_round->deaths);
    summarylogoutf("kills:%i", this_round->kills);
    summarylogoutf("delay_min:%i", this_round->delay_min);
    summarylogoutf("delay_max:%i", this_round->delay_max);

    // log events to csv
    formatstring(filename, "logs/%s_r%i_events.csv", game::player1->name, this_round->round_number);
    seteventlogfile(filename);
    // write header
    eventlogoutf(
        "timestamp;event_name;input_type;input_value;delay;shot_hit"
    );
    // write lines
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

    closeeventlogfile();
    closesummarylogfile();
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
        // x prevents overriding existing files and therefore previous logs by accident
        fname = findfile(fname, "wx"); 
        if(fname) eventlogfile = fopen(fname, "wx");
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
        fname = findfile(fname, "wx");
        if(fname) summarylogfile = fopen(fname, "wx");
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