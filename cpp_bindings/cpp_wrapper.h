#ifndef DSP_TOOLBOX_CPP_WRAPPER_H
#define DSP_TOOLBOX_CPP_WRAPPER_H

#include "../inc/waveform_gen.h"
#include <vector>

using namespace std;

class waveform_t {
  private:
	vector<float> t;
	unsigned int sample_rate;
	unsigned int wave_length;

  public:
	waveform_t(unsigned int _sample_rate, unsigned int _wave_length);
	~waveform_t();
};

class waveform_x {
  private:
	vector<float> x;
	vector<wave_properties_t> wave_properties;
	unsigned int wave_length;

  public:
	/* Functions to add new entries to any vector types */
	void add_wave_properties(float amplitude, float offset, float frequency,
							 float phase, HARMONIC_FUNC_E function_type);
	void gen_waveform();
	/* Constructors/destructors */
	waveform_x(unsigned int _wave_length);
	~waveform_x();
};

#endif /* DSP_TOOLBOX_CPP_WRAPPER_H */
