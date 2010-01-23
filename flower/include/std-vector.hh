/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2006--2010 Jan Nieuwenhuizen <janneke@gnu.org>

  LilyPond is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LilyPond is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STD_VECTOR_HH
#define STD_VECTOR_HH

#if 0

/*
  leads to dubious crashes - libstdc++  bug?
*/
#ifndef NDEBUG
#define _GLIBCXX_DEBUG 1
#endif
#endif

#include <algorithm>   /* find, reverse, sort */
#include <functional>  /* unary_function */
#include <cassert>
#include <string>

using namespace std;

#if HAVE_BOOST_LAMBDA_LAMBDA_HPP
#include <boost/lambda/lambda.hpp>
#endif

template<typename T>
int default_compare (T const &a, T const &b)
{
  if (a < b)
    return -1;
  else if (b < a)
    return 1;
  else
    return 0;
}

template<typename T>
int default_compare (T *const &a, T *const &b)
{
  if (a < b)
    return -1;
  else if (a > b)
    return 1;
  else
    return 0;
}

#include "compare.hh"

#ifndef VSIZE
#define VSIZE
typedef size_t vsize;
#define VPOS ((vsize) -1)
#endif

#if HAVE_STL_DATA_METHOD
#include <vector>
#else /* !HAVE_STL_DATA_METHOD */
#define vector __flower_vector
#include <vector>
#undef vector

namespace std {

  /* Interface without pointer arithmetic (iterator) semantics.  */
  template<typename T, typename A=std::allocator<T> >
  class vector : public __flower_vector<T, A>
  {
  public:
    typedef typename __flower_vector<T>::iterator iterator;
    typedef typename __flower_vector<T>::const_iterator const_iterator;

    vector<T, A> () : __flower_vector<T, A> ()
    {
    }

    vector<T, A> (vector<T, A> const& v) : __flower_vector<T, A> (v)
    {
    }

    vector<T, A> (const_iterator b, const_iterator e) : __flower_vector<T, A> (b, e)
    {
    }

    T*
    data ()
    {
      return &(*this)[0];
    }

    T const*
    data () const
    {
      return &(*this)[0];
    }
  };

} /* namespace std */

#endif /* !HAVE_STL_DATA_METHOD */

template<typename T>
T const &
boundary (vector<T> const &v, int dir, vsize i)
{
  assert (dir);
  return v[dir == -1 ? i : v.size () - 1 - i];
}

template<typename T>
T &
boundary (vector<T> &v, int dir, vsize i)
{
  assert (dir);
  return v[dir == -1 ? i : v.size () - 1 - i];
}

template<typename T>
T const &
back (vector<T> const &v, vsize i)
{
  return v[v.size () - i - 1];
}

template<typename T>
T&
back (vector<T> &v, vsize i)
{
  return v[v.size () - i - 1];
}

template<typename T>
void
concat (vector<T> &v, vector<T> const& w)
{
  v.insert (v.end (), w.begin (), w.end ());
}

template<typename T, typename Compare>
vsize
lower_bound (vector<T> const &v,
	     T const &key,
	     Compare less,
	     vsize b=0, vsize e=VPOS)
{
  if (e == VPOS)
    e = v.size ();
  typename vector<T>::const_iterator i = lower_bound (v.begin () + b,
						      v.begin () + e,
						      key,
						      less);

  return i - v.begin ();
}

template<typename T, typename Compare>
vsize
upper_bound (vector<T> const &v,
	     T const &key,
	     Compare less,
	     vsize b=0, vsize e=VPOS)
{
  if (e == VPOS)
    e = v.size ();

  typename vector<T>::const_iterator i = upper_bound (v.begin () + b,
						      v.begin () + e,
						      key,
						      less);

  return i - v.begin ();
}

template<typename T, typename Compare>
vsize
binary_search (vector<T> const &v,
	       T const &key,
	       Compare less=less<T> (),
	       vsize b=0, vsize e=VPOS)
{
  vsize lb = lower_bound (v, key, less, b, e);

  if (lb == v.size () || less (key, v[lb]))
    return VPOS;
  return lb;
}

template<typename T, typename Compare>
void
vector_sort (vector<T> &v,
	     Compare less,
	     vsize b=0, vsize e=VPOS)
{
  if (e == VPOS)
    e = v.size ();

  sort (v.begin () + b, v.begin () + e, less);
}

template<typename T>
void
reverse (vector<T> &v)
{
  // CHECKME: for a simple vector, like vector<int>, this should
  // expand to memrev.
  reverse (v.begin (), v.end ());
}

template<typename T>
void
uniq (vector<T> &v)
{
  v.erase (unique (v.begin (), v.end ()), v.end ());
}

template<typename T>
typename vector<T>::const_iterator
find (vector<T> const &v, T const &key)
{
  return find (v.begin (), v.end (), key);
}

#if HAVE_BOOST_LAMBDA_LAMBDA_HPP
#include <boost/lambda/lambda.hpp>
using namespace boost::lambda;
template<typename T>
void
junk_pointers (vector<T> &v)
{
  for_each (v.begin (), v.end (), (delete _1, _1 = 0));
  v.clear ();
}
#else

template<typename T> struct del : public unary_function<T, void>
{
  void operator() (T x)
  {
    delete x;
    x = 0;
  }
};

template<typename T>
void
junk_pointers (vector<T> &v)
{
  // Hmm.
  for_each (v.begin (), v.end (), del<T> ());
  v.clear ();
}
#endif /* HAVE_BOOST_LAMBDA */

vector<string> string_split (string str, char c);
string string_join (vector<string> const &strs, string infix);

#define iterof(i,s) typeof((s).begin()) i((s).begin())

#endif /* STD_VECTOR_HH */
