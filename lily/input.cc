/*
  input.cc -- implement Input

  source file of the LilyPond music typesetter

  (c) 1997--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "input.hh"

#include <cstdio>
using namespace std;

#include "international.hh"
#include "source-file.hh"
#include "sources.hh"
#include "warn.hh"

Input::Input (Input const &i)
{
  source_file_ = i.source_file_;
  start_ = i.start_;
  end_ = i.end_;
}

Input::Input ()
{
  source_file_ = 0;
  start_ = 0;
  end_ = 0;
}

Input
Input::spot () const
{
  return *this;
}

void
Input::set_spot (Input const &i)
{
  *this = i;
}

void
Input::step_forward ()
{
  if (end_ == start_)
    end_++;
  start_++;
}

void
Input::set_location (Input const &i_start, Input const &i_end)
{
  source_file_ = i_start.source_file_;
  start_ = i_start.start_;
  end_ = i_end.end_;
}

/*
  Produce GNU-compliant error message.  Correcting lilypond source is
  such a breeze if you ('re edidor) know (s) the error column too

  Format:

  [file:line:column:][warning:]message
*/
void
Input::message (string s) const
{
  if (source_file_)
    s = location_string () + ": " + s + "\n"
      + source_file_->quote_input (start_) + "\n";
  ::message (s);
}


void
Input::programming_error (string s) const
{
  message (_f ("programming error: %s", s.c_str ()));
  message (_ ("continuing, cross fingers") + "\n");
}


void
Input::warning (string s) const
{
  message (_f ("warning: %s", s));
}

void
Input::error (string s) const
{
  message (_f ("error: %s", s));
  // UGH, fix naming or usage
  // exit (1);
}

void
Input::non_fatal_error (string s) const
{
  message (_f ("error: %s", s));
}

string
Input::location_string () const
{
  if (source_file_)
    return source_file_->file_line_column_string (start_);
  return " (" + _ ("position unknown") + ")";
}

string
Input::line_number_string () const
{
  if (source_file_)
    return to_string (source_file_->get_line (start_));
  return "?";
}

string
Input::file_string () const
{
  if (source_file_)
    return source_file_->name_string ();
  return "";
}

int
Input::line_number () const
{
  if (source_file_)
    return source_file_->get_line (start_);
  return 0;
}

int
Input::column_number () const
{
  int line, chr, col = 0;
  source_file_->get_counts (start_, &line, &chr, &col);

  return col;
}

int
Input::end_line_number () const
{
  if (source_file_)
    return source_file_->get_line (end_);
  return 0;
}

int
Input::end_column_number () const
{
  int line, chr, col = 0;
  source_file_->get_counts (end_, &line, &chr, &col);

  return col;
}

void
Input::get_counts (int *line, int *chr, int *col) const
{
  source_file_->get_counts (start_, line, chr, col);
}

void
Input::set (Source_file *sf, char const *start, char const *end)
{
  source_file_ = sf;
  start_ = start;
  end_ = end;
}

Source_file *
Input::get_source_file () const
{
  return source_file_;
}

char const *
Input::start () const
{
  return start_;
}

char const *
Input::end () const
{
  return end_;
}
