#include <iostream>
#include <string.h>

#define DEBUG
#include "OSDPlib.h"
#include "log.h"


using namespace std;


int main( int argc, char** argv) {
    logInit(LOG_ALL,LOG_PRINT_TIME|LOG_PRINT_FILE|LOG_PRINT_LINE|LOG_PRINT_LEVEL_DESCRIPTION,NULL);
    if ( argc < 3){
        LOGMESG(LOG_ERROR, "Wrong input data");
    }
    if ( strcmp(argv[1], "store")==0){
        store_file( argv[2], "base_dir");
    }
    if ( strcmp(argv[1], "restore")==0){
        restore_file( argv[2],argv[3],  "base_dir");
    }
    return 0;
}