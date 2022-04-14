#ifndef LOGGER_H_
#define LOGGER_H_

#include <../shared/tools.h>
#include <sys/stat.h>
#include <sys/types.h>

struct round_event_data
{
    int timestamp;
    string ev_type;        // game event, input event
    // input events
    string input_type;     // MouseUp, KeyDown, etc
    string input_value;    // KEY_W, LMB
    int delay_this;
    // game events
    bool shot;                  // 1=hit, 0=miss, NULL=no shot
    bool bot_death;
    bool player_death;
};

struct game_round_data
{
    vector<round_event_data> events;
    int round_number;
    int kills;
    int deaths;
    float completion_time;
    int delay_min;
    int delay_max;
};



class Logger
{
private:
    const string dir = "../logs";

public:
    Logger();
    ~Logger();
    void create_dir();
    void write(string data);
};

#endif