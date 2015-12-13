#include <iostream>
#define DEBUG
#include "OSDPlib.h"
#include "log.h"


using namespace std;


int main() {
    logInit(LOG_ALL,LOG_PRINT_TIME|LOG_PRINT_FILE|LOG_PRINT_LINE|LOG_PRINT_LEVEL_DESCRIPTION,NULL);
    store_file( "a.out", "base_dir");
    return 0;
}