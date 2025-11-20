#ifndef DSP_TOOLBOX_CPP_WRAPPER_H
#define DSP_TOOLBOX_CPP_WRAPPER_H

#include "../inc/waveform_gen.h"
#include <vector>

using namespace std;

class waveform_t {
  private:
	unsigned int sample_rate;
	unsigned int wave_length;

  public:
  	vector<float> t;
	waveform_t(unsigned int _sample_rate, unsigned int _wave_length);
	// ~waveform_t();
};

class waveform_x {
  private:
	vector<wave_properties_t> wave_properties;
	unsigned int wave_length;

  public:
  	vector<float> x;
	/* Functions to add new entries to any vector types */
	void add_wave_properties(float amplitude, float offset, float frequency,
							 float phase, WAVEFORM_FUNC_E function_type);
	void gen_waveform(float *t);
	/* Constructors/destructors */
	waveform_x(unsigned int _wave_length);
	waveform_x(unsigned int _wave_length,
			   vector<wave_properties_t> _wave_properties);
	// ~waveform_x();
};

#endif /* DSP_TOOLBOX_CPP_WRAPPER_H */
