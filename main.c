#include <time.h>
#include "run.h"

int main() {
    clock_t tic = clock();
    run();
    clock_t tac = clock();
    printf("\n%f sec.\n", (double)(tac - tic) / CLOCKS_PER_SEC);
}
