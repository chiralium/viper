#include <time.h>
#include "run.h"
#include "test.h"

#define DEFAULT_FILENAME "/home/evgeniy/computer/lab/viper/cmake-build-debug/test.vi"

int main(int argc, char * argv[]) {
    if ( argc >= 2 ) {
        if ( !strcmp( argv[1], "-test" ) ) test();
        else if ( argv[1] ) run(argv[1]);
    } else {
        clock_t tic = clock();
        run(DEFAULT_FILENAME);
        clock_t tac = clock();
        printf("\n%f sec.\n", (double) (tac - tic) / CLOCKS_PER_SEC);
    }
}
