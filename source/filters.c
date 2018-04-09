#include "filters.h"

// ====  PROCEDURE:  BIQUAD_INIT  ====
// 

void biquad_init(t_biquad *biquad)
{
  if (!biquad) { return; }

  biquad->a0 = 1;
  biquad->a1 = 0;
  biquad->a2 = 0;
  biquad->b1 = 0;
  biquad->b2 = 0;

  biquad->freq = 0;
  biquad->gain = 1;
  biquad->q = 1;
  
  biquad->type = BQ_TYPE_IDENTITY;
  biquad->sym = gensym("identity");
  biquad->has_param = false;
  biquad->calc_coeff = NULL;

  biquad->samplerate = sys_getsr();
}

// ====  PROCEDURE:  BIQUAD_NEW  ====
// 

t_biquad *biquad_new(t_uint16 cnt)
{
  t_biquad *biquad = (t_biquad *)sysmem_newptr(sizeof(t_biquad) * cnt);
  if (!biquad) { return NULL; }

  for (t_uint8 bqd = 0; bqd < cnt; bqd++) { biquad_init(biquad + bqd); }

  return biquad;
}

// ====  PROCEDURE:  BIQUAD_RESIZE  ====
// 

t_biquad *biquad_resize(t_biquad *biquad, t_uint16 cnt)
{
  t_biquad *biquad_rsz = (t_biquad *)sysmem_resizeptr(biquad, sizeof(t_biquad) * cnt);
  if (!biquad_rsz) { return NULL; }

  for (t_uint8 bqd = 0; bqd < cnt; cnt++) { biquad_init(biquad_rsz + bqd); }

  return biquad_rsz;
}

// ====  PROCEDURE:  BIQUAD_FREE  ====
// 

void biquad_free(t_biquad *biquad)
{
  if (biquad) { sysmem_freeptr(biquad); }
}

// ====  PROCEDURE:  BIQUAD_SET_COEFF  ====
// 

void biquad_set_coeff(t_biquad *biquad_arr, t_uint16 cnt, t_atom *argv)
{
  if (!biquad_arr) { return; }

  t_atom *atom = argv;
  t_biquad *biquad = NULL;

  for (t_uint16 bqd = 0; bqd < cnt; bqd++) {
    biquad = biquad_arr + bqd;

    biquad->a0 = atom_getfloat(atom++);
    biquad->a1 = atom_getfloat(atom++);
    biquad->a2 = atom_getfloat(atom++);
    biquad->b1 = atom_getfloat(atom++);
    biquad->b2 = atom_getfloat(atom++);

    biquad->type = BQ_TYPE_UNDEFINED;
    biquad->sym = gensym("undefined");
    biquad->has_param = false;
    biquad->calc_coeff = NULL; }
}

// ====  PROCEDURE:  BIQUAD_SET_PARAM  ====
// 

void biquad_set_param(t_biquad *biquad_arr, t_double samplerate, t_uint16 cnt, t_atom *argv)
{
  if (!biquad_arr) { return; }

  t_atom *atom = argv;
  t_biquad *biquad = NULL;
  t_symbol *name;

  for (t_uint16 bqd = 0; bqd < cnt; bqd++) {
    biquad = biquad_arr + bqd;

    name = atom_getsym(atom++);

    if (name == gensym("lowpass")) {
      biquad->type = BQ_TYPE_LOWPASS;
      biquad->calc_coeff = biquad_lowpass_o; }

    else if (name == gensym("highpass")) {
      biquad->type = BQ_TYPE_HIGHPASS;
      biquad->calc_coeff = biquad_highpass_o; }

    else if (name == gensym("bandpass")) {
      biquad->type = BQ_TYPE_BANDPASS;
      biquad->calc_coeff = biquad_bandpass_o; }

    else if (name == gensym("bandstop")) {
      biquad->type = BQ_TYPE_BANDSTOP;
      biquad->calc_coeff = biquad_bandstop_o;  }

    else if (name == gensym("peaknotch")) {
      biquad->type = BQ_TYPE_PEAKNOTCH;
      biquad->calc_coeff = biquad_peaknotch_o; }

    else if (name == gensym("lowshelf")) {
      biquad->type = BQ_TYPE_LOWSHELF;
      biquad->calc_coeff = biquad_lowshelf_o; }

    else if (name == gensym("highshelf")) {
      biquad->type = BQ_TYPE_HIGHSHELF;
      biquad->calc_coeff = biquad_highshelf_o; }

    else if (name == gensym("resonant")) {
      biquad->type = BQ_TYPE_RESONANT;
      biquad->calc_coeff = biquad_resonant_o; }

    else if (name == gensym("allpass")) {
      biquad->type = BQ_TYPE_ALLPASS;
      biquad->calc_coeff = biquad_allpass_o; }

    else {
      biquad_init(biquad);
      continue;  }

    biquad->sym = name;
    biquad->has_param = true;

    biquad->freq = atom_getfloat(atom++);
    biquad->gain = atom_getfloat(atom++);
    biquad->q = atom_getfloat(atom++);

    biquad->samplerate = samplerate;
    biquad->calc_coeff(biquad); }
}

// ====  PROCEDURE:  BIQUAD_CHG_PARAM  ====
// 

void biquad_chg_param(t_biquad *biquad_arr, t_double samplerate, t_uint16 cnt, t_atom *argv)
{
  t_atom *atom = argv;
  t_biquad *biquad = biquad_arr;
  t_uint16 decr = cnt;

  while (decr--) {
    biquad->freq = atom_getfloat(atom++);
    biquad->gain = atom_getfloat(atom++);
    biquad->q = atom_getfloat(atom++);
    biquad->samplerate = samplerate;
    biquad->calc_coeff(biquad);
    biquad ++; }
}

// ====  PROCEDURE:  BIQUAD_UPD_COEFF  ====
// 

void biquad_update_sr(t_biquad *biquad_arr, t_double samplerate, t_uint16 cnt)
{
  t_biquad *biquad = biquad_arr;
  t_uint16 decr = cnt;

  while (decr--) {
    biquad->samplerate = samplerate;
    biquad->calc_coeff(biquad);
    biquad ++; }
}

// ====  PROCEDURE:  BIQUAD_LIST  ====
// 

void biquad_list(void *outl, t_biquad *biquad_arr, t_uint16 cnt)
{
  if (!biquad_arr) { return; }

  t_atom *mess_arr = (t_atom *)sysmem_newptr(sizeof(t_atom)* 5 * cnt);
  if (!mess_arr) { return; }
  
  t_atom *atom = mess_arr;
  t_biquad *biquad = NULL;
  for (t_uint16 bqd = 0; bqd < cnt; bqd++) {
    biquad = biquad_arr + bqd;

    atom_setfloat(atom++, biquad->a0);
    atom_setfloat(atom++, biquad->a1);
    atom_setfloat(atom++, biquad->a2);
    atom_setfloat(atom++, biquad->b1);
    atom_setfloat(atom++, biquad->b2); }

  outlet_list(outl, gensym(""), 5 * cnt, mess_arr);
  sysmem_freeptr(mess_arr);
}

// ====  PROCEDURE:  BIQUAD_POST  ====
// 

void biquad_post(void *x, t_biquad *biquad_arr, t_uint16 cnt)
{
  if (!biquad_arr) { return; }

  t_biquad *biquad = NULL;
  for (t_uint16 bqd = 0; bqd < cnt; bqd++) {

    biquad = biquad_arr + bqd;

    POST("Biquad:  Type: %s - a0 = %f - a1 = %f - a2 = %f - b1 = %f - b2 = %f", biquad->sym->s_name, biquad->a0, biquad->a1, biquad->a2, biquad->b1, biquad->b2);
    if (biquad->has_param) { POST("  Freq: %f - Gain: %f - Q: %f - Samplerate: %f", biquad->freq, biquad->gain, biquad->q, biquad->samplerate); } }
}

// Apply the filter from an input buffer to an output buffer
// No need to store the X(n-1) and X(n-2) values
void biquad_apply_IO(t_biquad *biquad, float *in, float *out, t_uint8 ch_cnt, t_uint32 fr_cnt)
{
  if (fr_cnt < 2) {  return; }

  for (t_uint8 ch = 0; ch < ch_cnt; ch++) {
    out[ch] = (float)(float)biquad->a0 * in[ch];
    out[ch_cnt + ch] = (float)biquad->a0 * in[ch_cnt + ch] + (float)biquad->a1 * in[ch] - (float)biquad->b1 * out[ch]; }

  for (t_uint32 fr = 2; fr < fr_cnt; fr++) {
    for (t_uint8 ch = 0; ch < ch_cnt; ch++) {
      out[fr * ch_cnt + ch] = (float)biquad->a0 * in[fr * ch_cnt + ch] + (float)biquad->a1 * in[(fr - 1) * ch_cnt + ch] + (float)biquad->a2 * in[(fr - 2) * ch_cnt + ch]
        - (float)biquad->b1 * out[(fr - 1) * ch_cnt + ch] - (float)biquad->b2 * out[(fr - 2) * ch_cnt + ch];
    }
  }
}

// Apply the filter directly in a buffer
// Necessary to store the X(n-1) and X(n-2) values
void biquad_apply(t_biquad *biquad_arr, t_uint16 bqd_cnt, float *out, t_uint8 ch_cnt, t_uint32 fr_cnt)
{
  if (fr_cnt < 2) {  return; }

  float *x_m1 = (float *)sysmem_newptr(sizeof(float) * ch_cnt);
  float *x_m2 = (float *)sysmem_newptr(sizeof(float) * ch_cnt);
  float *p_tmp, tmp;

  if ((!x_m1) || (!x_m2)) { return; }

  t_biquad *biquad;
  for (t_uint16 bqd = 0; bqd < bqd_cnt; bqd++) {

    biquad = biquad_arr + bqd;

    for (t_uint8 ch = 0; ch < ch_cnt; ch++) {
      x_m2[ch] = out[ch];
      out[ch] = (float)biquad->a0 * out[ch];

      x_m1[ch] = out[ch_cnt + ch];
      out[ch_cnt + ch] = (float)biquad->a0 * x_m1[ch] + (float)biquad->a1 * x_m2[ch] - (float)biquad->b1 * out[ch]; }

    for (t_uint32 fr = 2; fr < fr_cnt; fr++) {
      for (t_uint8 ch = 0; ch < ch_cnt; ch++) {
        tmp = out[fr * ch_cnt + ch];
        out[fr * ch_cnt + ch] = (float)biquad->a0 * out[fr * ch_cnt + ch] + (float)biquad->a1 * x_m1[ch] + (float)biquad->a2 * x_m2[ch]
          - (float)biquad->b1 * out[(fr - 1) * ch_cnt + ch] - (float)biquad->b2 * out[(fr - 2) * ch_cnt + ch];
        x_m2[ch] = tmp;  }

      p_tmp = x_m1; x_m1 = x_m2; x_m2 = p_tmp; } }

  sysmem_freeptr(x_m1);
  sysmem_freeptr(x_m2);
}

void biquad_lowpass_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_q, norm;

  A      = pow(10, biquad->gain / 20);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_q = tan_2 / biquad->q;
  tan_2 *= tan_2;
  norm  = 1 / (1 + tan_q + tan_2);
  
  biquad->a0 = tan_2 * norm * A;
  biquad->a1 = 2 * biquad->a0;
  biquad->a2 = biquad->a0;
  biquad->b1 = 2 * (tan_2 - 1) * norm;
  biquad->b2 = (1 - tan_q + tan_2) * norm;
}

void biquad_highpass_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_q, norm;

  A      = pow(10, biquad->gain / 20);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_q = tan_2 / biquad->q;
  tan_2 *= tan_2;
  norm  = 1 / (1 + tan_q + tan_2);

  biquad->a0 = norm * A;
  biquad->a1 = -2 * biquad->a0;
  biquad->a2 = biquad->a0;
  biquad->b1 = 2 * (tan_2 - 1) * norm;
  biquad->b2 = (1 - tan_q + tan_2) * norm;
}

void biquad_bandpass_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_q, norm;

  A = pow(10, biquad->gain / 20);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_q = tan_2 / biquad->q;
  tan_2 *= tan_2;
  norm = 1 / (1 + tan_q + tan_2);

  biquad->a0 = tan_q * norm * A;
  biquad->a1 = 0;
  biquad->a2 = -biquad->a0;
  biquad->b1 = 2 * (tan_2 - 1) * norm;
  biquad->b2 = (1 - tan_q + tan_2) * norm;
}

void biquad_bandstop_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_q, norm;

  A = pow(10, biquad->gain / 20);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_q = tan_2 / biquad->q;
  tan_2 *= tan_2;
  norm = 1 / (1 + tan_q + tan_2);

  biquad->b1 = 2 * (tan_2 - 1) * norm;
  biquad->b2 = (1 - tan_q + tan_2) * norm;

  biquad->a0 = (1 + tan_2) * norm * A;
  biquad->a1 = biquad->b1 * A;
  biquad->a2 = biquad->a0;
}

void biquad_peaknotch_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_q_A, tan_q_iA, norm;

  A = pow(10, biquad->gain / 40);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_q_A = tan_2 / biquad->q;
  tan_q_iA = tan_q_A / A;
  tan_q_A *= A;
  tan_2 *= tan_2;
  norm = 1 / (1 + tan_q_iA + tan_2);

  biquad->a0 = (1 + tan_q_A + tan_2) * norm;
  biquad->a1 = -2 * (1 - tan_2) * norm;
  biquad->a2 = (1 - tan_q_A + tan_2) * norm;
  biquad->b1 = biquad->a1;
  biquad->b2 = (1 - tan_q_iA + tan_2) * norm;
}

void biquad_lowshelf_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_sqrt, norm;

  A = pow(10, biquad->gain / 40);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_sqrt = tan_2 * sqrt((1 + A * A) * (1 / biquad->q - 1) + 2 * A);
  tan_2 *= tan_2;
  norm = 1 / (A + tan_sqrt + tan_2);

  biquad->a0 = A * (1 + tan_sqrt + A * tan_2) * norm;
  biquad->a1 = -2 * A * (1 - A * tan_2) * norm;
  biquad->a2 = A * (1 - tan_sqrt + A * tan_2) * norm;
  biquad->b1 = -2 * (A - tan_2) * norm;
  biquad->b2 = (A - tan_sqrt + tan_2) * norm;
}

void biquad_highshelf_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_sqrt, norm;

  A = pow(10, biquad->gain / 40);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_sqrt = tan_2 * sqrt((1 + A * A) * (1 / biquad->q - 1) + 2 * A);
  tan_2 *= tan_2;
  norm = 1 / (1 + tan_sqrt + A * tan_2);

  biquad->a0 = A * (A + tan_sqrt + tan_2) * norm;
  biquad->a1 = -2 * A * (A - tan_2) * norm;
  biquad->a2 = A * (A - tan_sqrt + tan_2) * norm;
  biquad->b1 = -2 * (1 - A * tan_2) * norm;
  biquad->b2 = (1 - tan_sqrt + A * tan_2) * norm;
}

void biquad_resonant_o(t_biquad *biquad)
{
  t_double A, tan_, tan_2, tan_q, norm;

  A = pow(10, biquad->gain / 20);
  tan_ = tan(PI * biquad->freq / biquad->samplerate);
  tan_q = tan_ / biquad->q;
  tan_2 = tan_ * tan_;
  norm = 1 / (1 + tan_q + tan_2);

  biquad->a0 = tan_ * norm * A;
  biquad->a1 = 0;
  biquad->a2 = -biquad->a0;
  biquad->b1 = 2 * (tan_2 - 1) * norm;
  biquad->b2 = (1 - tan_q + tan_2) * norm;
}

void biquad_allpass_o(t_biquad *biquad)
{
  t_double A, tan_2, tan_q, norm;

  A = pow(10, biquad->gain / 20);
  tan_2 = tan(PI * biquad->freq / biquad->samplerate);
  tan_q = tan_2 / biquad->q;
  tan_2 *= tan_2;
  norm = 1 / (1 + tan_q + tan_2);

  biquad->b1 = -2 * (1 - tan_2) * norm;
  biquad->b2 = (1 - tan_q + tan_2) * norm;

  biquad->a0 = biquad->b2 * A;
  biquad->a1 = biquad->b1 * A;
  biquad->a2 = A;
}

void biquad_lowpass_s(t_biquad *biquad)
{
  t_double ampl, w0, cos_, alpha, norm;

  ampl = pow(10, biquad->gain / 20);
  w0 = 2 * PI * biquad->freq / biquad->samplerate;
  cos_ = cos(w0);
  alpha = sin(w0) / (2 * biquad->q);
  norm = 1 / (1 + alpha);

  biquad->a0 = ampl * norm * (1 - cos_) / 2;
  biquad->a1 = 2 * biquad->a0;
  biquad->a2 = biquad->a0;
  biquad->b1 = -2 * norm * cos_;
  biquad->b2 = norm * (1 - alpha);
}

void biquad_highpass_s(t_biquad *biquad)
{
  t_double V, K, K_K, K_Q, norm;

  V = pow(10, biquad->gain / 20);
  K = tan(PI * biquad->freq / biquad->samplerate);

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (1 + K_Q + K_K);

  biquad->a0 = norm * V;
  biquad->a1 = -2 * biquad->a0;
  biquad->a2 = biquad->a0;
  biquad->b1 = 2 * (K_K - 1) * norm;
  biquad->b2 = (1 - K_Q + K_K) * norm;
}

void biquad_bandpass_s(t_biquad *biquad)
{
  t_double V, K, K_K, K_Q, norm;

  V = pow(10, biquad->gain / 20);
  K = tan(PI * biquad->freq / biquad->samplerate);

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (1 + K_Q + K_K);

  biquad->a0 = K_Q * norm * V;
  biquad->a1 = 0;
  biquad->a2 = -biquad->a0;
  biquad->b1 = 2 * (K_K - 1) * norm;
  biquad->b2 = (1 - K_Q + K_K) * norm;
}

void biquad_bandstop_s(t_biquad *biquad)
{
  t_double V, K, K_K, K_Q, norm;

  V = pow(10, biquad->gain / 20);
  K = tan(PI * biquad->freq / biquad->samplerate);

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (1 + K_Q + K_K);

  biquad->a0 = (1 + K_K) * norm * V;
  biquad->a1 = 2 * (K_K - 1) * norm * V;
  biquad->a2 = biquad->a0;
  biquad->b1 = biquad->a1;
  biquad->b2 = (1 - K_Q + K_K) * norm;
}

void biquad_peaknotch_s(t_biquad *biquad)
{
  t_double V, V1, V2, K, K_K, K_Q, norm;

  V = pow(10, biquad->gain / 20);
  V1 = MAX(1, V);
  V2 = MAX(1, 1 / V);
  K = tan(PI * biquad->freq / biquad->samplerate);

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (1 + V2 * K_Q + K_K);

  biquad->a0 = (1 + V1 * K_Q + K_K) * norm;
  biquad->a1 = 2 * (K_K - 1) * norm;
  biquad->a2 = (1 - V1 * K_Q + K_K) * norm;
  biquad->b1 = biquad->a1;
  biquad->b2 = (1 - V2 * K_Q + K_K) * norm;
}

void biquad_lowshelf_s(t_biquad *biquad)
{
  t_double V, V1, V2, r_2_V1_K, r_2_V2_K, K, K_K, K_Q, norm;

  K = tan(PI * biquad->freq / biquad->samplerate);
  V = pow(10, biquad->gain / 20);
  V1 = MAX(1, V);
  V2 = MAX(1, 1 / V);
  r_2_V1_K = sqrt(2 * V1) * K;
  r_2_V2_K = sqrt(2 * V2) * K;

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (1 + r_2_V2_K + V2 * K_K);

  biquad->a0 = (1 + r_2_V1_K + V1 * K_K) * norm;
  biquad->a1 = 2 * (V1 * K_K - 1) * norm;
  biquad->a2 = (1 - r_2_V1_K + V1 * K_K) * norm;
  biquad->b1 = 2 * (V2 * K_K - 1) * norm;
  biquad->b2 = (1 - r_2_V2_K + V2 * K_K) * norm;
}

void biquad_highshelf_s(t_biquad *biquad)
{
  t_double V, V1, V2, r_2_V1_K, r_2_V2_K, K, K_K, K_Q, norm;

  K = tan(PI * biquad->freq / biquad->samplerate);
  V = pow(10, biquad->gain / 20);
  V1 = MAX(1, V);
  V2 = MAX(1, 1 / V);
  r_2_V1_K = sqrt(2 * V1) * K;
  r_2_V2_K = sqrt(2 * V2) * K;

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (V2 + r_2_V2_K + K_K);

  biquad->a0 = (V1 + r_2_V1_K + K_K) * norm;
  biquad->a1 = 2 * (K_K - V1) * norm;
  biquad->a2 = (V1 - r_2_V1_K + K_K) * norm;
  biquad->b1 = 2 * (K_K - V2) * norm;
  biquad->b2 = (V2 - r_2_V2_K + K_K) * norm;
}

// XXX == TO DO

void biquad_resonant_s(t_biquad *biquad)
{
  t_double V, V1, V2, r_2_V1_K, r_2_V2_K, K, K_K, K_Q, norm;

  K = tan(PI * biquad->freq / biquad->samplerate);
  V = pow(10, biquad->gain / 20);
  V1 = MAX(1, V);
  V2 = MAX(1, 1 / V);
  r_2_V1_K = sqrt(2 * V1) * K;
  r_2_V2_K = sqrt(2 * V2) * K;

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (V2 + r_2_V2_K + K_K);

  biquad->a0 = (V1 + r_2_V1_K + K_K) * norm;
  biquad->a1 = 2 * (K_K - V1) * norm;
  biquad->a2 = (V1 - r_2_V1_K + K_K) * norm;
  biquad->b1 = 2 * (K_K - V2) * norm;
  biquad->b2 = (V2 - r_2_V2_K + K_K) * norm;
}

// XXX == TO DO

void biquad_allpass_s(t_biquad *biquad)
{
  t_double V, V1, V2, r_2_V1_K, r_2_V2_K, K, K_K, K_Q, norm;

  K = tan(PI * biquad->freq / biquad->samplerate);
  V = pow(10, biquad->gain / 20);
  V1 = MAX(1, V);
  V2 = MAX(1, 1 / V);
  r_2_V1_K = sqrt(2 * V1) * K;
  r_2_V2_K = sqrt(2 * V2) * K;

  K_K = K * K;
  K_Q = K / biquad->q;
  norm = 1 / (V2 + r_2_V2_K + K_K);

  biquad->a0 = (V1 + r_2_V1_K + K_K) * norm;
  biquad->a1 = 2 * (K_K - V1) * norm;
  biquad->a2 = (V1 - r_2_V1_K + K_K) * norm;
  biquad->b1 = 2 * (K_K - V2) * norm;
  biquad->b2 = (V2 - r_2_V2_K + K_K) * norm;
}