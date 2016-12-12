#include "Weather.h"
#include <unistd.h>

int main() {

    Weather weather;

	weather.Start();

	sleep(30);

    return 0;
}

