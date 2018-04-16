//******************************************************************************
//  @file
//  y.buff.filter - applies filters to a buffer
//  Yves Candau - ycandau@gmail.com
//
//  @ingroup  myExternals
//

// ========  HEADER FILES  ========

// Max headers
// Called in max_util.h

// Personal headers
#include "max_util.h"
#include "buff_util.h"
#include "filters.h"

// ========  DEFINES  ========

#define BIQUAD_CNT 5;

// ========  TYPEDEF AND CONST GLOBAL VARIABLES  ========


// ========  STRUCTURE DECLARATION  ========

typedef struct _buff_filter {

  t_object obj;

  void* outl_mess;            // Outlet 0: General messages
  void* outl_bang;            // Outlet 1: Bang on task completion

  t_buff_info* buff;          // Buffer to be filtered
  t_biquad*    biquad_arr;    // Array of biquad filters
  t_uint16      biquad_cnt;    // Number of biquad filters
  t_bool       mute;          // Mute the posting on information

} t_buff_filter;

// ========  FUNCTION PROTOTYPES  ========

void* buff_filter_new    (t_symbol* sym, long argc, t_atom* argv);
void  buff_filter_free   (t_buff_filter* x);
void  buff_filter_assist (t_buff_filter* x, void* b, long msg, long arg, char* str);

t_max_err buff_filter_notify (t_buff_filter* x, t_symbol* sender_sym, t_symbol* msg, void* sender_ptr, void* data);

void  buff_filter_eq        (t_buff_filter* x, t_symbol* sym);
void  buff_filter_post      (t_buff_filter* x, t_symbol* sym);
void  buff_filter_set_coeff (t_buff_filter* x, t_symbol* sym, long argc, t_atom* argv);
void  buff_filter_set_param (t_buff_filter* x, t_symbol* sym, long argc, t_atom* argv);
void  buff_filter_mute      (t_buff_filter* x,  long n);

void  buff_filter_bang     (t_buff_filter* x);
void  buff_filter_int      (t_buff_filter* x, long n);
void  buff_filter_float    (t_buff_filter* x, t_double f);
void  buff_filter_anything (t_buff_filter* x, t_symbol* sym, long argc, t_atom* argv);
void  buff_filter_dblclick (t_buff_filter* x);
void  buff_filter_acant    (t_buff_filter* x);

// ========  GLOBAL CLASS POINTER AND STATIC VARIABLES  ========

void* buff_filter_class;

static t_symbol* sym_empty;

// ========  INITIALIZATION ROUTINE  ========

int C74_EXPORT main(void) {

  t_class* c;

  c = class_new("y.buff.filter~", (method)buff_filter_new, (method)buff_filter_free,
    (long)sizeof(t_buff_filter), 0L /* leave NULL!! */, A_GIMME, 0);

  class_addmethod(c, (method)buff_filter_assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)buff_filter_notify, "notify", A_CANT, 0);

  class_addmethod(c, (method)buff_filter_eq, "eq", A_SYM, 0);
  class_addmethod(c, (method)buff_filter_post, "post", A_SYM, 0);
  class_addmethod(c, (method)buff_filter_mute, "mute", A_LONG, 0);

  class_addmethod(c, (method)buff_filter_set_coeff, "set_coeff", A_GIMME, 0);
  class_addmethod(c, (method)buff_filter_set_param, "set_param", A_GIMME, 0);

  class_addmethod(c, (method)buff_filter_bang, "bang", 0);
  class_addmethod(c, (method)buff_filter_int, "int", A_LONG, 0);
  class_addmethod(c, (method)buff_filter_float, "float", A_FLOAT,  0);
  class_addmethod(c, (method)buff_filter_anything, "anything", A_GIMME, 0);
  class_addmethod(c, (method)buff_filter_dblclick, "dblclick", A_CANT, 0);

  class_register(CLASS_BOX, c);
  buff_filter_class = c;

  sym_empty = gensym("");

  return 0;
}

// ========  NEW INSTANCE ROUTINE: BUFF_FILTER_NEW  ========
// New method for the buff_filter object

void* buff_filter_new(t_symbol* sym, long argc, t_atom* argv) {

  t_buff_filter* x = NULL;
  x = (t_buff_filter*)object_alloc(buff_filter_class);

  if (!x) {
    MY_ERR("buff_filter_new:  Object allocation failed.");
    goto BUFF_FILTER_NEW_ERROR;
  }

  TRACE("buff_filter_new");

  // Create outlets
  x->outl_bang = bangout((t_object*)x);           // Outlet 1: Bang on task completion
  x->outl_mess = outlet_new((t_object*)x, NULL);  // Outlet 0: General messages

  // Buffer structure
  x->buff = buff_new();
  if (!x->buff) {
    MY_ERR("buff_filter_new:  Memory allocation failed for buff_new.");
    goto BUFF_FILTER_NEW_ERROR;
  }

  // Biquad array
  x->biquad_cnt = 0;
  x->biquad_arr = biquad_new(5);
  if (!x->biquad_arr) {
    MY_ERR("buff_filter_new:  Memory allocation failed for biquad_new.");
    goto BUFF_FILTER_NEW_ERROR;
  }

  // Various
  x->mute = true;

  // Post a creation message
  POST("New object created.");

  return(x);

BUFF_FILTER_NEW_ERROR:
  buff_filter_free(x);
  return NULL;
}

// ========  PROCEDURE: BUFF_FILTER_FREE  ========
// Free method for the buff_filter object

void buff_filter_free(t_buff_filter* x) {

  TRACE("buff_filter_free");

  // If the object is undefined exit
  if (!x) { return; }

  // Free the buffer structure
  if (x->buff) { buff_free(x->buff); }

  // Free the biquad array
  if (x->biquad_arr) { biquad_free(x->biquad_arr); }
}

  // ====  PROCEDURE: BUFF_FILTER_ASSIST  ====

void buff_filter_assist(t_buff_filter* x, void* b, long msg, long arg, char* str) {

  TRACE("buff_filter_assist");

  if (msg == ASSIST_INLET) {
    switch (arg) {
    case 0: sprintf(str, "Inlet 0: All purpose (list)"); break;
    default: break;
  }
}

  else if (msg == ASSIST_OUTLET) {
    switch (arg) {
    case 0: sprintf(str, "Outlet 0: General messages (list)"); break;
    case 1: sprintf(str, "Outlet 1: Bang on task completion (bang)"); break;
    default: break;
  }
}
}

// ========  METHOD: BUFF_FILTER_NOTIFY  ========
// Intercept notifications to check if they are from the buffer

t_max_err buff_filter_notify(t_buff_filter* x, t_symbol* sender_sym, t_symbol* msg, void* sender_ptr, void* data) {

  TRACE("buff_filter_notify");

  // Get the class name of the notifying object
  t_symbol* class_name = object_classname(data);

  // If the object sending the notification is a buffer
  if (class_name == gensym("buffer~")) {

    // Get the name of the buffer sending the notification
    t_atom ret_ato;
    object_method_typed(data, gensym("getname"), 0, NULL, &ret_ato);
    t_symbol* notif_buff_sym = atom_getsym(&ret_ato);

    if (!x->mute) {
      POST("notify:  Buffer \"%s\" - %s", notif_buff_sym->s_name, msg->s_name);
    }

    if (notif_buff_sym == x->buff->sym) {
      return buffer_ref_notify(x->buff->ref, sender_sym, msg, sender_ptr, data);
    }
  }

  // In all other cases
  else if (!x->mute) { POST("notify:  class: %s - message: %s", class_name->s_name, msg->s_name); }

  return 0;
}

// ====  PROCEDURE: BUFF_FILTER_EQ  ====
//******************************************************************************
//
//
void buff_filter_eq(t_buff_filter* x, t_symbol* sym) {

  TRACE("buff_filter_eq");

  buff_get_obj(x, x->buff, sym);
  buff_get_param(x->buff);

  MY_ASSERT(!x->buff->has_obj, "Unable to find buffer \"%s\".", sym->s_name);
  MY_ASSERT(!x->buff->has_file, "Buffer \"%s\" is empty.", sym->s_name);

  float* buff = buffer_locksamples(x->buff->obj);
  biquad_update_sr(x->biquad_arr, x->buff->msr * 1000, x->biquad_cnt);
  biquad_apply(x->biquad_arr, x->biquad_cnt, buff, x->buff->ch_cnt, x->buff->fr_cnt);
  buffer_unlocksamples(x->buff->obj);
  buffer_setdirty(x->buff->obj);
  outlet_bang(x->outl_bang);
}

// ====  PROCEDURE: BUFF_FILTER_POST  ====

void buff_filter_post(t_buff_filter* x, t_symbol* sym) {

  TRACE("buff_filter_post");

  if (sym == gensym("biquads")) {
    biquad_post(x, x->biquad_arr, x->biquad_cnt);
  }

  else {
    buff_get_obj(x, x->buff, sym);
    buff_get_param(x->buff);
    buff_post(x, x->buff);
  }
}

// ====  PROCEDURE: BUFF_FILTER_SET_COEFF  ====

void buff_filter_set_coeff(t_buff_filter* x, t_symbol* sym, long argc, t_atom* argv) {

  TRACE("buff_filter_set_coeff");

  if ((argc == 0) || ((argc % 5) != 0) || (argc > 25)) {
    MY_ERR("%s:  Up to five sets of five floats expected.", sym->s_name);
    return;
  }

  x->biquad_cnt = (t_uint16)(argc / 5);
  biquad_set_coeff(x->biquad_arr, x->biquad_cnt, argv);
}

// ====  PROCEDURE: BUFF_FILTER_SET_PARAM  ====

void buff_filter_set_param(t_buff_filter* x, t_symbol* sym, long argc, t_atom* argv) {

  TRACE("buff_filter_set_param");

  if ((argc == 0) || ((argc % 4) != 0) || (argc > 20)) {
    MY_ERR("%s:  Up to five sets of [sym, freq, gain, q] expected.", sym->s_name);
    return;
  }

  x->biquad_cnt = (t_uint16)(argc / 4);
  biquad_set_param(x->biquad_arr, sys_getsr(), x->biquad_cnt, argv);
}

// ====  PROCEDURE: BUFF_FILTER_MUTE  ====

void buff_filter_mute(t_buff_filter* x, long is_mute) {

  if (is_mute == 1) { x->mute = true; }
  else if (is_mute == 0)  { x->mute = false; }
  else { MY_ERR("mute:  Arg 0:  Invalid argument. Should be: 0 or 1."); }
}

// ====  PROCEDURE: BUFF_FILTER_BANG  ====

void buff_filter_bang(t_buff_filter* x) {

  TRACE("buff_filter_bang");
}

// ====  PROCEDURE: BUFF_FILTER_INT  ====

void buff_filter_int(t_buff_filter* x, long n) {

  TRACE("buff_filter_int");
}

// ====  PROCEDURE: BUFF_FILTER_FLOAT  ====

void buff_filter_float(t_buff_filter* x, t_double f) {

  TRACE("buff_filter_float");
}

// ====  PROCEDURE: BUFF_FILTER_ANYTHING  ====

void buff_filter_anything(t_buff_filter* x, t_symbol* sym, long argc, t_atom* argv) {

  TRACE("buff_filter_anything");
  WARNING("The message \"%s\" is not recognized.", sym->s_name);
}

// ====  PROCEDURE: BUFF_FILTER_DBLCLICK  ====

void buff_filter_dblclick(t_buff_filter* x) {

  TRACE("buff_filter_dblclick");
}

// ====  PROCEDURE: BUFF_FILTER_ACANT  ====

void buff_filter_acant(t_buff_filter* x) {

  TRACE("buff_filter_acant");
}
