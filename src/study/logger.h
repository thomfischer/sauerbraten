#ifndef LOGGER_H
#define LOGGER_H

namespace study
{
    struct round_event
    {
        long int timestamp;
        // input events
        char* input_type;     // MouseUp, KeyDown, etc
        char* input_value;    // KEY_W, LMB
        int delay_this;
        // game events
        char* event_name;
        bool shot_hit;             // 1=hit, 0=miss, NULL=no shot
    };

    struct game_round
    {
        vector<round_event> events;
        int round_number;
        int kills;
        int deaths;
        float completion_time;
        int delay_min;
        int delay_max;
    };

    struct condition
    {
        int roundnumber;
        int baselatency;
        int minvariance;
        int maxvariance;
    };

    FILE *studylogfile;
    game_round* this_round;
    vector<game_round>* rounds;

    FILE* getstudylogfile();
    void closestudylogfile();
    void setstudylogfile(const char *fname);
    void studylogoutf(const char *fmt, ...);
    void studylogoutfv(const char *fmt, va_list args);
    // void writelog(FILE *file, const char *buf);
    // void writelogv(FILE *file, const char *fmt, va_list args);


    uint64_t epoch_time_ms();
    game_round* get_this_round();
    vector<game_round>* get_all_rounds();
    
    void reset_round_struct();

    void init();

    void write_to_file();
};
#endif