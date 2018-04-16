#ifndef YC_BUFF_UTIL_H_
#define YC_BUFF_UTIL_H_

// ========  HEADER FILE FOR MISCELLANEOUS MAX UTILITIES  ========

#include "max_util.h"
#include "buffer.h"

// ====  TYPEDEF ENUM AND STRUCT  ====

typedef enum {

  BUFF_NO_LINK,   // Buffer has not been linked to
  BUFF_NO_SYM,    // Failed to get a symbol for the buffer
  BUFF_NO_REF,    // Failed to get a reference for the buffer
  BUFF_NO_OBJ,    // Failed to get an object for the buffer
  BUFF_NO_FILE,   // Failed to load a file in the buffer
  BUFF_READY      // Buffer is succesfully linked to and a file has been loaded into it

} t_buff_state;

typedef struct _buff_info {

  t_symbol*     sym;   // Name
  t_buffer_ref* ref;   // Buffer reference
  t_buffer_obj* obj;   // Buffer object

  t_uint8   ch_cnt;   // Number of channels
  t_uint32  fr_cnt;   // Length in frames
  t_double msr;      // Samplerate in ms

  t_bool has_obj;
  t_bool has_file;
  t_bool is_changed;   // To indicate when the buffer has changed

} t_buff_info;

// ====  PROCEDURE DECLARATIONS  ====

t_buff_info* buff_new       ();
void         buff_free      (t_buff_info* buff);
void         buff_reset     (t_buff_info* buff);
void         buff_get_obj   (void* x, t_buff_info* buff, t_symbol* buff_sym);
void         buff_get_param (t_buff_info* buff);
void         buff_post      (void* x, t_buff_info* buff);

// ========  END OF HEADER FILE  ========

#endif
