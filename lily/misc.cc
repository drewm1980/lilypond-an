/*
  misc.cc -- implement various stuff

  source file of the GNU LilyPond music typesetter

  (c) 1997--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
  Jan Nieuwenhuizen <janneke@gnu.org>
*/


#include "misc.hh"

/*
  Return the 2-log, rounded down
*/
int
intlog2 (int d)
{
  assert (d);
  int i = 0;
  while ((d != 1))
    {
      d /= 2;
      i++;
    }

  assert (! (d / 2));
  return i;
}

double
log_2 (double x)
{
  return log (x) / log (2.0);
}

Real
directed_round (Real f, Direction d)
{
  if (d < 0)
    return floor (f);
  else
    return ceil (f);
}



/*
   0 at threshold,  1 at 0, with 1/x falloff.
 */
Real
peak_around (Real epsilon,  Real threshold, Real x)
{
  if (x < 0)
    return 1.0;
  return max (- epsilon * (x - threshold) / ((x + epsilon) * threshold), 0.0);
}

/*
  0 at 0,  1 at standard_x, and increasing thereafter. 
 */
Real
convex_amplifier (Real standard_x, Real increase_factor, Real x)
{
  return (exp (increase_factor * x / standard_x) - 1.0) / (exp (increase_factor) - 1.0); 
}

string
camel_case_to_lisp_identifier (string in)
{
  vector<char> out;
  
  /* don't add '-' before first character */
  out.push_back (char (tolower (in[0])));
    
  for (size_t inpos = 1; inpos < in.size (); inpos++)
    {
      if (isupper (in[inpos]))
	out.push_back ('-');
      out.push_back ( char(tolower (in[inpos])));
    }
  
  string result (&out[0], out.size ());
  replace_all (&result, '_', '-');

  return result;
}

