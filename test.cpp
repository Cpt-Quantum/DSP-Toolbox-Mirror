#include <iostream>
#include "cpp_bindings/cpp_wrapper.h"
#include "inc/waveform_gen.h"

using namespace std;

int main()
{
	float time[32];
	gen_time(time, 32, 100);
	for(int i = 0; i < 32; i++)
		cout << time[i];

	return 0;
}
