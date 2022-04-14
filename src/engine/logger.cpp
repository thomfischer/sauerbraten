#include "logger.h"

Logger::Logger()
{
    create_dir();
    //f = fopen((dir + "/" + filename).c_str(), "w+");
}

Logger::~Logger()
{
    //fclose(f);
}

void Logger::create_dir()
{
    // create dir if it doesn't exist
    // https://stackoverflow.com/a/7430262
    struct stat st = {0};
    if (stat(dir, &st) == -1) 
    {
        mkdir(dir, 0700);
    }
}

void Logger::write(string data)
{

}