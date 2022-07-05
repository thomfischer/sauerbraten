#ifndef LOGGER_H
#define LOGGER_H

namespace study
{

// public declarations are inside shared/iengine.h

// private
const char* con_cfg_fname = "conditions.txt";
int roundnumber = -1;   // first round start will increment it to 0
bool can_shoot = true;  // related functions exist to prevent automatic fire on holding LMB

FILE* condition_config = NULL;
FILE* eventlogfile = NULL;
FILE* summarylogfile = NULL;

participant* this_participant = new participant;
vector<participant> participants;

game_round* this_round = new game_round;
vector<game_round> rounds;

void logoutfv(FILE* f, const char *fmt, va_list args);
void logoutf(FILE* f, const char *fmt, ...);
void load_participant(string playername);
void update_delaydaemon_FIFO(condition con);
condition load_condition();
void load_conditions_file();
void closelogfiles();
void setfiles(string participant);
void getroundnumber();
bool prev_round_files_valid(int roundnumber);

};

#endif