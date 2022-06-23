#include "engine.h"
#include "logger.h"

#define LOGSTRLEN 512

namespace study
{

game_round* get_this_round() { return this_round; }
vector<game_round> get_all_rounds() { return rounds; }

void logoutfv(FILE* f, const char *fmt, va_list args)
{
    if(f) writelogv(f, fmt, args);
}

void logoutf(FILE* f, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    logoutfv(f, fmt, args);
    va_end(args);
}

void old_logoutfv(const char *fmt, va_list args)
{
    FILE *f = getlogfile();
    if(f) writelogv(f, fmt, args);
}

void old_logoutf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    old_logoutfv(fmt, args);
    va_end(args);
}

// playername = P1, P2, etc.
void load_participant(string playername)
{
    delete this_participant;
    int playernum;
    sscanf(playername, "%i", &playernum);
    if(playernum > participants.length())
    {
        conoutf(CON_ERROR, "\f3ERROR: Participant ID higher than allowed!");
    }
    loopv(participants)
    {
        if(participants[i].id == playernum)
        {
            this_participant = &participants[i];
            return;
        }
    }

    conoutf(CON_ERROR, "\f3ERROR: Participant ID not found!");
}

// update the file used by the DelayDaemon to change latency
void update_delaydaemon_FIFO(condition con)
{
    string buf;
    sprintf(buf, "%i %i %i %i", con.baselatency, con.maxlatency, 0, 0);
    FILE *f, *f2;

    const char* fname = findfile("delaydaemonmouse", "w+");
    if(fname) f = fopen(fname, "w+");
    if(f == 0) return;
    study::logoutf(f, buf);
    fclose(f);

    const char* fname2 = findfile("delaydaemonkeyboard", "w+");
    if(fname2) f2 = fopen(fname2, "w+");
    if(f2 == 0) return;
    study::logoutf(f2, buf);
    fclose(f2);
}

condition load_condition()
{
    condition con = this_participant->conditions[roundnumber];
    update_delaydaemon_FIFO(con);
    return con;
}

void load_conditions_file()
{
    string line;
    stream* st = openutf8file(con_cfg_fname, "r");
    while(st->getline(line, sizeof(line)))
    {
        if(line[0] == 'P')
        {
            participant p;
            sscanf(line, "P%i", &p.id);
            participants.add(p);
        }
        else
        {
            participant* p = &participants.last();
            int roundnumber, basedelay, maxdelay;
            sscanf(line, "%i %i %i", &roundnumber, &basedelay, &maxdelay);
            p->conditions.add( {roundnumber, basedelay, maxdelay} );
        }
    }
}

void closelogfiles()
{
    if(eventlogfile)
    {
        fclose(eventlogfile);
        eventlogfile = NULL;
    }
    if(summarylogfile)
    {
        fclose(summarylogfile);
        summarylogfile = NULL;
    }
}

void write_log_to_file()
{
    // overwrite possibly unset variables with -1; would be sizeof var otherwise
    if(this_round->round_start < 0)    this_round->round_start = -1;
    if(this_round->round_end < 0)      this_round->round_end = -1;
    if(this_round->kills < 0)          this_round->kills = -1;
    if(this_round->deaths < 0)         this_round->deaths = -1;
    if(this_round->baselatency < 0)    this_round->baselatency = -1;
    if(this_round->maxlatency < 0)     this_round->maxlatency = -1;

    // write summary log file with all values but the events
    study::logoutf(summarylogfile,  "round_start:%li",   this_round->round_start);
    study::logoutf(summarylogfile,  "round_end:%li",     this_round->round_end);
    study::logoutf(summarylogfile,  "deaths:%i",         this_round->deaths);
    study::logoutf(summarylogfile,  "kills:%i",          this_round->kills);
    study::logoutf(summarylogfile,  "delay_min:%i",      this_round->baselatency);
    study::logoutf(summarylogfile,  "delay_max:%i",      this_round->maxlatency);

    // log events to csv
    // write header
    study::logoutf(
        eventlogfile,
        "timestamp;event_name"
    );
    // write lines
    for(int i=0; i<this_round->events.length(); ++i)
    {
        round_event ev = this_round->events[i];

        study::logoutf(
            eventlogfile,
            "%li;%s",
            ev.timestamp,
            ev.event_name
        );
    }

    closelogfiles();
}

uint64_t epoch_time_ms()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long int ms = time.tv_sec * 1000 + time.tv_usec / 1000;
    return ms;
}

void setfiles(string participant)
{
    string sumfilename, evfilename;

    closelogfiles();

    formatstring(sumfilename, "logs/p%s_r%i_sum.txt", participant, roundnumber);
    if(sumfilename)
    {
        strcpy(sumfilename, findfile(sumfilename, "wx"));
        if(sumfilename) summarylogfile = fopen(sumfilename, "wx");
        if(!summarylogfile)
        {
            conoutf(CON_ERROR, "\f3ERROR: summary file for this participant and round already exists!");
        }
    }

    formatstring(evfilename, "logs/p%s_r%i_log.csv", participant, roundnumber);
    if(evfilename)
    {
        strcpy(evfilename, findfile(evfilename, "wx"));
        if(evfilename) eventlogfile = fopen(evfilename, "wx");
        if(!eventlogfile)
        {
            conoutf(CON_ERROR, "\f3ERROR: event file for this participant and round already exists!");
        }
    }
}

void init_new_round(string playername)
{
    roundnumber++;

    // if it's the first (0) round, load a new participant
    if(roundnumber == 0) load_participant(playername);

    delete this_round;
    this_round = new game_round;
    condition con = load_condition();
    this_round->baselatency = con.baselatency;
    this_round->maxlatency = con.maxlatency;

    setfiles(playername);
}

void set_can_shoot(bool b) { can_shoot = b; }
bool get_can_shoot() { return can_shoot; }

void init()
{
    load_conditions_file();
}

}