#ifndef PHYS_SERVICES_GSL_SPRNG_H_INCLUDED
#define PHYS_SERVICES_GSL_SPRNG_H_INCLUDED

#include <sprng/sprng.h>
/**
 * gsl-sprng.h
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @brief SPRNG Implementation of the I3RandomService interface.  
 * This implementation uses a combination of SPRNG and GSL to generate
 * statistically independent streams of pseudo-random number distributions.
 * SPRNG (Scalable Pseudo-Random Number Generator) is a library developed by
 * Michael Mascagni, et al at Florida State University, http://sprng.cs.fsu.edu
 *
 * The purpose of this implementation is to provide an I3RandomService that
 * can be used in a distributed computing environment and ensuring little
 * correlation between parallel streams of numbers.
 *
 * The basic approach for this is based on code from Darren Wilkinson
 * https://darrenjw.github.io/ who wrote an MPI simple interface
 * that doesn't make use of streams.
 *
 * @version $Revision$
 * @date $Date$
 * @author juancarlos
 *
 * @todo 
 */


typedef struct
{
   int seed;
   int streamnum;
   int nstreams;
   int *streamptr;
   char *savedstate; // saved rng state (from previous run).
   uint64_t icalls; // number of calls to isprng()
   uint64_t dcalls; // number of calls to sprng()
} gsl_sprng_stream;


static void sprng_set(void * vstate,unsigned long int s)
{
  gsl_sprng_stream *stream = (gsl_sprng_stream*) vstate;

  if (stream->savedstate) {
   		stream->streamptr = unpack_sprng(stream->savedstate);
  } else {
  	stream->seed = s;
  	stream->streamptr = init_sprng(
		DEFAULT_RNG_TYPE, 
  		stream->streamnum, 
		stream->nstreams, 
		stream->seed,
		SPRNG_DEFAULT);
  } 
}

static unsigned long sprng_get(void * vstate)
{
  gsl_sprng_stream *stream = (gsl_sprng_stream*) vstate;
  stream->icalls++;
  return( (long) isprng(stream->streamptr) );
}

static double sprng_get_double(void * vstate)
{
  gsl_sprng_stream *stream = (gsl_sprng_stream*) vstate;
  stream->dcalls++;
  return( (double) sprng(stream->streamptr));
}

// rootcint 4.04.02 barfs on this.  stupid, stupid.
#ifndef __CINT__
static const gsl_rng_type gsl_rng_sprng20 =
  {"sprng20",        	/* name */
   0x7fffffffUL,     	/* RAND_MAX */
   0,                	/* RAND_MIN */
   0,                	/* size of state - not sure about this */
   &sprng_set,          /* initialisation */
   &sprng_get,          /* get integer RN */
   &sprng_get_double  	/* get double RN */
};
#endif

inline gsl_rng *gsl_sprng_init(int seed, int nstreams, int streamnum, char *state=NULL,
    uint64_t icalls=0, uint64_t dcalls=0)
{
   gsl_sprng_stream* s = 
   	  (gsl_sprng_stream*)  malloc( sizeof( gsl_sprng_stream ) );

   if (s == 0) {
       GSL_ERROR_VAL (
	   	"failed to allocate space for gsl_sprng_stream struct",
                     GSL_ENOMEM, 0);
   };

   /*Allocate memory for stream parameters*/
   s->seed		= seed;
   s->streamnum = streamnum;
   s->nstreams 	= nstreams;
   s->savedstate = state;
   s->icalls = icalls;
   s->dcalls = dcalls;

   /*Initialize stream*/
   if (state) {
   		s->streamptr = unpack_sprng(state);
   } else {
   		s->streamptr = 
   			init_sprng(
				DEFAULT_RNG_TYPE, 
				streamnum, 
				nstreams, 
				seed, 
				SPRNG_DEFAULT);
   }

   /*Allocate memory for rng parameters*/
   gsl_rng *r 	= (gsl_rng *) malloc (sizeof (gsl_rng));
   if (r == 0) {
       GSL_ERROR_VAL ("failed to allocate space for rng struct",
                        GSL_ENOMEM, 0);
	   free(s);
   };

   /*Set rng parameters*/
   r->type    = &gsl_rng_sprng20;
   r->state   = s;

   return r;
}

/**
 * save the current state of the RNG to an array of characters
 * @param rng gsl_rng struct
 * @param state array to store state in
 * @return size of state array
 */
inline int gsl_sprng_pack(gsl_rng* rng, char **state)
{
   gsl_sprng_stream* s = (gsl_sprng_stream*) rng->state;
   return pack_sprng(s->streamptr, state);
}

/**
 * Free memory for RNG
 * @param r gsl_rng struct
 */
inline void gsl_sprng_free (gsl_rng * r)
{
  free (r->state);
  free (r);
}

#endif
