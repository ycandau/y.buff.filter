#include "buff_util.h"

// ====  PROCEDURE:  BUFF_NEW  ====

t_buff_info* buff_new() {

  t_buff_info* buff = (t_buff_info*)sysmem_newptr(sizeof(t_buff_info));
  if (buff == NULL) { return NULL; }

  buff->sym = gensym("");
  buff->ref = NULL;
  buff->obj = NULL;

  buff->ch_cnt = 0;
  buff->fr_cnt = 0;
  buff->msr = 0;

  buff->has_obj = false;
  buff->has_file = false;
  buff->is_changed = false;

  return buff;
}

// ====  PROCEDURE:  BUFF_FREE  ====
//

void buff_free(t_buff_info* buff) {

  if (buff->ref) { object_free(buff->ref); }
  if (buff)       { sysmem_freeptr(buff); }
}

// ====  PROCEDURE:  BUFF_RESET  ====
//

void buff_reset(t_buff_info* buff) {

  ;
}

// ====  PROCEDURE:  BUFF_GET_OBJ  ====
//

void buff_get_obj(void* x, t_buff_info* buff, t_symbol* buff_sym) {

  // Test that the symbol for the buffer name is not empty
  if (buff_sym == gensym("")) {
    buff->has_obj = false;
    buff->has_file = false;
    return;
  }
  else { buff->sym = buff_sym; }

  // If the buffer reference already exists, change it
  if (buff->ref) { buffer_ref_set(buff->ref, buff->sym); }
  // Otherwise create a new buffer reference
  else { buff->ref = buffer_ref_new((t_object*)x, buff->sym); }

  // Test that the buffer reference was successfully changed or created
  if (buff->ref == NULL) {
    buff->has_obj = false;
    buff->has_file = false;
    return;
  }

  //==== Buffer objects ====

  // Create a new buffer object from the buffer reference
  buff->obj = buffer_ref_getobject(buff->ref);

  // Test that the buffer object was succesfully retrieved
  if (buff->obj == NULL) {
    buff->has_obj = false;
    buff->has_file = false;
    return;
  }

  else {
    buff->has_obj = true;
    return;
  }
}

// ====  PROCEDURE:  BUFF_GET_PARAM  ====
//

void buff_get_param(t_buff_info* buff) {

  // If the buffer has no object
  if (buff->has_obj == false) {
    buff->has_file = false;
    return;
  }

  // Get the buffer variables
  buff->fr_cnt = (t_uint32)buffer_getframecount(buff->obj);
  buff->ch_cnt = (t_uint8)buffer_getchannelcount(buff->obj);
  buff->msr     = buffer_getmillisamplerate(buff->obj);

  // Test the buffer variables
  if ((buff->fr_cnt == 0) || (buff->ch_cnt == 0) || (buff->msr == 0)) {
    buff->has_file = false;
  }

  // Otherwise the buffer is linked and a file is loaded.
  else {
    buff->has_file = true;
  }
}

// ====  PROCEDURE:  BUFF_POST  ====
//

void buff_post(void* x, t_buff_info* buff) {

  if (buff->has_obj == false) { POST("Buffer \"%s\" not found.", buff->sym->s_name); }
  else {
    POST("Buffer \"%s\":  Channels: %i - Frames: %i - Samplerate (smp / ms): %f",
      buff->sym->s_name, buff->ch_cnt, buff->fr_cnt, buff->msr);
    }
}
