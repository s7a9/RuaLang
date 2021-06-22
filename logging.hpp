#include <ctime>  
#include <iostream>  
#include <fstream> 
#include <direct.h>
#ifndef __EASYLOG_PIPI_0813
#define __EASYLOG_PIPI_0813

namespace EasyLog
{
    static char* log_path = nullptr;

    static void Write(std::string log) {  
        if (!log_path) return;
        try
        {    
            std::ofstream ofs;  
            time_t t = time(0);  
            char tmp[64];  
            strftime(tmp, sizeof(tmp), "[%Y-%m-%d %X]", localtime(&t));  
            ofs.open(log_path, std::ofstream::app);  
            
            ofs << tmp << " - ";  
            ofs.write(log.c_str(), log.size());  
            ofs << std::endl;  
            ofs.close();         
        }
        catch(...)
        {
        } 
    }
};

#endif