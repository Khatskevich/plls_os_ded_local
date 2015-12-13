// The same as log/log.c with only one difference. It uses separate process writer against pthread.
// It is better ( I think ) in case of main program crushing.
#define DEBUG

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "log.h"

#define LOG_STARTED 1
#define LOG_STOPPED 0

typedef struct
{
    int isStarted ;
    int logDes;
    unsigned flags;
    int logLevel;
    int writeBufDes;
    int readBufDes;
    pthread_t writerThreadId;
    struct timeval startTime;
} LOGMAININFO;

LOGMAININFO logMainInfo = {.isStarted = LOG_STOPPED };
int writerProcId;

void * threadWriter( void* param);


int logInit(unsigned logLevel, unsigned flags, const char * filename){
    int des;
    int rc;
    int pipefd[2];// I am using pipe as circular buffer. It is thread safe.
    rc = __sync_val_compare_and_swap(&logMainInfo.isStarted, LOG_STOPPED, LOG_STARTED);
    if ( rc == LOG_STARTED ){
        LOGMESG(LOG_ERROR, "LOG IS CURRENTLY STARTED!");
        logChangeLvl(logLevel);
        logChangeFlags(flags);
        return -1;
    }

    if ( logLevel >= LOG_LEVELS_COUNT || logLevel < LOG_ALL ){
        goto log_exit_0;
    }

    if (filename == NULL){
        des = 2; // stderr
    } else{
        des = open(filename, O_APPEND | O_CREAT | O_WRONLY, 0666);
        if ( des == -1 )
        {
            goto log_exit_0;
        }
    }
    rc = pipe( pipefd );
    if ( rc == -1 ){
        goto log_exit_1;
    }

    logMainInfo.flags = flags;
    logMainInfo.readBufDes = pipefd[0] ;
    logMainInfo.writeBufDes = pipefd[1] ;
    logMainInfo.logLevel = logLevel ;
    logMainInfo.logDes = des ;
    logMainInfo.isStarted = 1;
    gettimeofday(&logMainInfo.startTime,0);

    rc = fork();
    if ( rc == 0 ){ //child
        close(logMainInfo.writeBufDes);
        threadWriter( NULL);
    }else if ( rc > 0){ // parent
        writerProcId = rc;
        close(logMainInfo.readBufDes);
    }else{
        close( pipefd[1]);
        goto log_exit_1;
    }
    LOGMESG(LOG_ERROR,"Log started successfully.");
    return 0;

    log_exit_2:
    close( pipefd[0]);
    close( pipefd[1]);
    log_exit_1:
    close( des);
    log_exit_0:
    logMainInfo.isStarted = LOG_STOPPED;
    return -1;
}

int logClose(){
    int rc;
    if ( logMainInfo.isStarted == 0){
        return -1;
    }
    logMainInfo.isStarted = 0;
    close( logMainInfo.writeBufDes );
    waitpid( writerProcId , &rc , 0 );
    close( logMainInfo.readBufDes );
    close( logMainInfo.logDes);
    if( logMainInfo.logDes != 2){ //stderr
        close( logMainInfo.logDes );
    }
    return 0;
}

// thread which is writing data out
void * threadWriter( void* param){
    char buf[WRITER_ATOM_SIZE];
    ssize_t len;
    ssize_t writtenlen;
    while( len = read( logMainInfo.readBufDes, buf, WRITER_ATOM_SIZE )){
        writtenlen = write( logMainInfo.logDes, buf, len );
        if( writtenlen != len );
    }
    close( logMainInfo.readBufDes);
    close( logMainInfo.logDes);
    exit(0);
}

float timedifference_msec(struct timeval t0, struct timeval t1)
{    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f; }


void logCheckAndInit(){
    int rc;
    rc = __sync_val_compare_and_swap(&logMainInfo.isStarted, LOG_STOPPED, LOG_STOPPED);
    if ( rc == LOG_STOPPED ){
        rc = logInit( LOG_INFO , LOG_PRINT_LEVEL_DESCRIPTION | LOG_PRINT_FILE | LOG_PRINT_LINE , NULL);
    }
}

// main logging function
int logMesg( const char *fname, int lineno ,const char* group, int priority ,const char* str,...)
{
    logCheckAndInit();
    if ( priority < logMainInfo.logLevel ){
        return 0;
    }
    char buf[MAX_MESG_SIZE];
    int len_preamb = 0;
    int len_mesg;
    ssize_t len;
    va_list argptr;
    va_start(argptr, str);
    if ( logMainInfo.flags & LOG_PRINT_TIME ){
        struct timeval t1;
        float elapsed;
        gettimeofday(&t1, 0);
        elapsed = timedifference_msec( logMainInfo.startTime, t1);
        len_preamb+=snprintf( buf + len_preamb, MAX_MESG_SIZE-1-len_preamb, "[%0.3f]",  elapsed);
    }
    if ( logMainInfo.flags & LOG_PRINT_LEVEL_DESCRIPTION ){
        len_preamb+=snprintf( buf + len_preamb, MAX_MESG_SIZE-1-len_preamb,
                              "%s:", LOGLEVELS_DESCRIPTIONS[priority]);
    }
    if ( logMainInfo.flags & LOG_PRINT_GROUP ){
        len_preamb+=snprintf( buf + len_preamb, MAX_MESG_SIZE-1-len_preamb, "%s:", group);
    }
    if ( logMainInfo.flags & LOG_PRINT_FILE ){
        len_preamb+=snprintf(  buf + len_preamb, MAX_MESG_SIZE-1-len_preamb, "%s:", fname);
    }
    if ( logMainInfo.flags & LOG_PRINT_LINE ){
        len_preamb+=snprintf(  buf + len_preamb, MAX_MESG_SIZE-1-len_preamb, "%d:",  lineno);
    }
    len_mesg = vsnprintf( buf+len_preamb, MAX_MESG_SIZE-len_preamb-1 , str ,argptr);
    buf[len_preamb+len_mesg] = '\n';
    len = write( logMainInfo.writeBufDes, buf, len_preamb + len_mesg + 1);
    if ( len != len_preamb + len_mesg + 1 );
    return len+1;
}

int logChangeLvl( unsigned logLevel )
{
    logCheckAndInit();
    if ( logLevel >= LOG_LEVELS_COUNT || logLevel < LOG_ALL ){
        return -1;
    }
    logMainInfo.logLevel = logLevel;
    return 0 ;
}

int logChangeFlags( unsigned flags )
{
    logCheckAndInit();
    logMainInfo.flags = flags;
    return 0 ;
}