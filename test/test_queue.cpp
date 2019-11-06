#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;

struct Point {
  float temperature,
        humidity,
        timestamp;
};

Point circular_average_points(Point pt[], size_t count, size_t npts, size_t bufflen) {
	// Average previous points 
	Point avg({0, 0, 0});
	for (int k = 0; k < npts; k++) {
		int index = (count-k) % bufflen;
		avg.temperature += pt[index].temperature / npts;
		avg.humidity += pt[index].humidity / npts;
		avg.timestamp += pt[index].timestamp / npts;
	}
	return(avg);
}

Point sample_sensor() {
	Point pt({0, 0, 0});
	return pt;
}


int main() {
	srand (time(NULL));

	Point ptMinute[60], ptHour[24], ptDay[100];


	ptMinute[50].temperature = 101;
	ptHour[0] = circular_average_points(ptMinute, 50, 10, 60);
	cout << ptHour[0].temperature << '\t' << ptHour[0].humidity << '\t' << ptHour[0].timestamp << endl;

	int countMinute = 55;
	for (int k = 1; k <= 10; k++) {
		Point pt = ptMinute[(countMinute - k) % 60];

		cout << pt.temperature << '\t' << pt.humidity << '\t' << pt.timestamp << endl;
	}
	
	
	return 0;
}