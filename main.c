#include <time.h>
#include "run.h"
#include "test.h"

int main(int argc, char * argv[]) {
    if ( argc >= 2 ) {
        if ( !strcmp( argv[1], "-test" ) ) test();
    } else {
        clock_t tic = clock();
        run();
        clock_t tac = clock();
        printf("\n%f sec.\n", (double) (tac - tic) / CLOCKS_PER_SEC);
    }
}
