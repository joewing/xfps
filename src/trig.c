
#include "xfps.h"

const float PI = 3.14159265359;

float sine[360];
float cosine[360];

void InitializeTrig() {
	int x;

	for(x = 0; x < 360; x++) {
		sine[x] = sin(((float)x * PI) / 180.0);
		cosine[x] = cos(((float)x * PI) / 180.0);
	}


}


