#include "Weather.h"
#include <unistd.h>

int main() {

    Weather weather;

	weather.Start();

	sleep(20);

	weather.Stop();

    return 0;
}

