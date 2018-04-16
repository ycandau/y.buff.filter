#ifndef YC_FILTERS_H_
#define YC_FILTERS_H_

// ========  HEADER FILE FOR MISCELLANEOUS MAX UTILITIES  ========

#include "max_util.h"

// ====  TYPEDEF ENUM AND STRUCT  ====

typedef enum {

  BQ_TYPE_UNDEFINED,
  BQ_TYPE_IDENTITY,
  BQ_TYPE_LOWPASS,
  BQ_TYPE_HIGHPASS,
  BQ_TYPE_BANDPASS,
  BQ_TYPE_BANDSTOP,
  BQ_TYPE_PEAKNOTCH,
  BQ_TYPE_LOWSHELF,
  BQ_TYPE_HIGHSHELF,
  BQ_TYPE_RESONANT,
  BQ_TYPE_ALLPASS,

} t_biquad_type;

typedef struct _biquad t_biquad;

struct _biquad {

  t_double a0;
  t_double a1;
  t_double a2;
  t_double b1;
  t_double b2;

  t_double gain;
  t_double freq;
  t_double q;

  t_biquad_type type;
  t_symbol* sym;
  t_bool has_param;
  t_double samplerate;

  void(*calc_coeff)(t_biquad* biquad);
};

// ====  PROCEDURE DECLARATIONS  ====

void       biquad_init      (t_biquad* biquad);
t_biquad*  biquad_new       (t_uint16 cnt);
t_biquad*  biquad_resize    (t_biquad* biquad, t_uint16 cnt);
void       biquad_free      (t_biquad* biquad);
void       biquad_set_coeff (t_biquad* biquad_arr, t_uint16 cnt, t_atom* argv);
void       biquad_set_param (t_biquad* biquad_arr, t_double samplerate, t_uint16 cnt, t_atom* argv);
void       biquad_chg_param (t_biquad* biquad_arr, t_double samplerate, t_uint16 cnt, t_atom* argv);
void       biquad_update_sr (t_biquad* biquad_arr, t_double samplerate, t_uint16 cnt);

void       biquad_list     (void* outl, t_biquad* biquad_arr, t_uint16 cnt);
void       biquad_post     (void* x, t_biquad* biquad_arr, t_uint16 cnt);
void       biquad_apply_IO (t_biquad* biquad, float* in, float* out, t_uint8 ch_cnt, t_uint32 fr_cnt);
void       biquad_apply    (t_biquad* biquad, t_uint16 bqd_cnt, float* out, t_uint8 ch_cnt, t_uint32 fr_cnt);

void biquad_lowpass_o   (t_biquad* biquad);
void biquad_highpass_o  (t_biquad* biquad);
void biquad_bandpass_o  (t_biquad* biquad);
void biquad_bandstop_o  (t_biquad* biquad);
void biquad_peaknotch_o (t_biquad* biquad);
void biquad_lowshelf_o  (t_biquad* biquad);
void biquad_highshelf_o (t_biquad* biquad);
void biquad_resonant_o  (t_biquad* biquad);
void biquad_allpass_o   (t_biquad* biquad);

void biquad_lowpass_s   (t_biquad* biquad);
void biquad_highpass_s  (t_biquad* biquad);
void biquad_bandpass_s  (t_biquad* biquad);
void biquad_bandstop_s  (t_biquad* biquad);
void biquad_peaknotch_s (t_biquad* biquad);
void biquad_lowshelf_s  (t_biquad* biquad);
void biquad_highshelf_s (t_biquad* biquad);
void biquad_resonant_s  (t_biquad* biquad);
void biquad_allpass_s   (t_biquad* biquad);

// ========  END OF HEADER FILE  ========

#endif
