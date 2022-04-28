#include "engine.h"

#define LOGSTRLEN 512

namespace study
{

FILE* condition_config = NULL;
const char* con_cfg_fname = "conditions.cfg";
FILE* eventlogfile = NULL;
FILE* summarylogfile = NULL;
game_round* this_round = new game_round;
vector<game_round>* rounds = new vector<game_round>;

participant* this_participant = new participant;
vector<participant>* participants = new vector<participant>;

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

void setfiles(string playername)
{
    string filename;
    formatstring(filename, "logs/%s_r%i_sum.txt", playername, this_round->round_number);
    if(setsummarylogfile(filename) == 0)
    {
        conoutf(CON_ERROR, "\f3ERROR: file for this participant and round already exists!");
    }

    formatstring(filename, "logs/%s_r%i_events.csv", playername, this_round->round_number);
    if(seteventlogfile(filename) == 0)
    {
        conoutf(CON_ERROR, "\f3ERROR: file for this participant and round already exists!");
    }
}

void load_conditions()
{
    string line;
    stream* st = openutf8file(con_cfg_fname, "r");
    while(st->getline(line, sizeof(line)))
    {
        if(line[0] == 'P')
        {
            participant p;;
            p.id = (int) line[1];
            participants->add(p);
        }
        else
        {
            participant p = participants->last();
            int vals[4];
            // TODO slice string. assign vals via slices. add p to vector.
        }
        
    }
}

void write_to_file()
{
    // overwrite possibly unset variables with -1; would be sizeof var otherwise
    if(!this_round->round_start) this_round->round_start = -1;
    if(!this_round->round_end) this_round->round_end = -1;
    if(!this_round->kills) this_round->kills = -1;
    if(!this_round->deaths) this_round->deaths = -1;
    if(!this_round->delay_max) this_round->delay_max = -1;
    if(!this_round->delay_min) this_round->delay_min = -1;

    // write summary log file with all values but the events
    summarylogoutf("round_start:%li", this_round->round_start);
    summarylogoutf("round_end:%li", this_round->round_end);
    summarylogoutf("deaths:%i", this_round->deaths);
    summarylogoutf("kills:%i", this_round->kills);
    summarylogoutf("delay_min:%i", this_round->delay_min);
    summarylogoutf("delay_max:%i", this_round->delay_max);

    // log events to csv
    // write header
    eventlogoutf(
        "timestamp;event_name;input_type;input_value;delay;shot_hit"
    );
    // write lines
    for(int i=0; i<this_round->events.length(); ++i)
    {
        round_event ev = this_round->events[i];

        // drop all inputs made before or after rounds (e.g. in menus)
        if(ev.input_value
        && (ev.timestamp > this_round->round_end
            || ev.timestamp < this_round->round_start))
        { continue; }

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
    logoutf("finito");

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

int seteventlogfile(const char *fname)
{
    closeeventlogfile();
    if(fname && fname[0])
    {
        // x prevents overriding existing files and therefore previous logs by accident
        fname = findfile(fname, "wx"); 
        conoutf(fname);
        if(fname) eventlogfile = fopen(fname, "wx");
        if(summarylogfile == 0) return 0;
    }
    FILE *f = geteventlogfile();
    if(f) setvbuf(f, NULL, _IOLBF, BUFSIZ);
    return 1;
}

FILE* getsummarylogfile()
{
    return summarylogfile;
}

int setsummarylogfile(const char *fname)
{
    closesummarylogfile();
    if(fname && fname[0])
    {
        fname = findfile(fname, "wx");
        if(fname) summarylogfile = fopen(fname, "wx");
        if(summarylogfile == 0) return 0;
    }
    FILE *f = getsummarylogfile();
    if(f) setvbuf(f, NULL, _IOLBF, BUFSIZ);
    return 1;
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