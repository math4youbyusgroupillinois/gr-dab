/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * config.h is generated by configure.  It contains the results
 * of probing for features, options etc.  It should be the first
 * file included in your .cc file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dab_select_vectors_vbb.h>
#include <gr_io_signature.h>
#include <gr_expj.h>

#define M_TWOPI (2*M_PI)

/*
 * Create a new instance of dab_select_vectors_vbb and return
 * a boost shared_ptr.  This is effectively the public constructor.
 */
dab_select_vectors_vbb_sptr 
dab_make_select_vectors_vbb (unsigned int vlen, unsigned int num, unsigned int skip)
{
  return dab_select_vectors_vbb_sptr (new dab_select_vectors_vbb (vlen, num, skip));
}

dab_select_vectors_vbb::dab_select_vectors_vbb (unsigned int vlen, unsigned int num, unsigned int skip) : 
  gr_block ("select_vectors_vbb",
             gr_make_io_signature2 (2, 2, sizeof(char)*vlen, sizeof(char)),
             gr_make_io_signature2 (2, 2, sizeof(char)*vlen, sizeof(char))),
  d_vlen(vlen), d_num(num), d_skip(skip), d_index(0)
{
  assert(d_num!=0);
  assert(d_vlen!=0);
}

void 
dab_select_vectors_vbb::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  int in_req  = noutput_items * (d_skip+d_num)/d_num; /* very coarse .. */

  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = in_req;
}

int 
dab_select_vectors_vbb::general_work (int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items)
{
  const char *iptr = (const char *) input_items[0];
  const char *trigger = (const char *) input_items[1];
  
  char *optr = (char *) output_items[0];
  char *triggerout = (char *) output_items[1];

  int n_consumed = 0;
  int n_produced = 0;


  while (n_consumed < ninput_items[0] && n_consumed < ninput_items[1] && n_produced < noutput_items) {

    /* start of frame? */
    if (*trigger++ == 1)
      d_index=0;
    
    /* select this vector? */
    if (d_index >= d_skip && d_index < d_num + d_skip) {
      /* trigger signal */
      if (d_index == d_skip)
        *triggerout++ = 1;
      else
        *triggerout++ = 0;
      /* data */
      memcpy(optr, iptr, d_vlen);
      iptr += d_vlen;
      optr += d_vlen;
      n_produced++;
    } else {
      iptr += d_vlen;
    }
  
    d_index++;
    n_consumed++;
  }

  consume_each(n_consumed);
  return n_produced;
}
