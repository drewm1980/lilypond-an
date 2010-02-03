#!@PYTHON@
# -*- coding: utf-8 -*-
# extract_texi_filenames.py

# USAGE:  extract_texi_filenames.py [-o OUTDIR] FILES
#
# -o OUTDIR specifies that output files should rather be written in OUTDIR
#
# Description:
# This script parses the .texi file given and creates a file with the
# nodename <=> filename/anchor map.
# The idea behind: Unnumbered subsections go into the same file as the
# previous numbered section, @translationof gives the original node name,
# which is then used for the filename/anchor.
#
# If this script is run on a file texifile.texi, it produces a file
# texifile[.LANG].xref-map with tab-separated entries of the form
#        NODE\tFILENAME\tANCHOR
# LANG is the document language in case it's not 'en'
# Note: The filename does not have any extension appended!
# This file can then be used by our texi2html init script to determine
# the correct file name and anchor for external refs

import sys
import re
import os
import getopt

optlist, args = getopt.getopt (sys.argv[1:],'o:')
files = args

outdir = '.'
for x in optlist:
    if x[0] == '-o':
        outdir = x[1]

if not os.path.isdir (outdir):
    if os.path.exists (outdir):
        os.unlink (outdir)
    os.makedirs (outdir)

include_re = re.compile (r'@include ((?!../lily-).*?)\.texi$', re.M)
whitespaces = re.compile (r'\s+')
section_translation_re = re.compile ('^@(node|(?:unnumbered|appendix)\
(?:(?:sub){0,2}sec)?|top|chapter|(?:sub){0,2}section|\
(?:major|chap|(?:sub){0,2})heading|translationof) (.*?)\\s*$', re.MULTILINE)

def expand_includes (m, filename):
    filepath = os.path.join (os.path.dirname (filename), m.group(1)) + '.texi'
    if os.path.exists (filepath):
        return extract_sections (filepath)[1]
    else:
        print "Unable to locate include file " + filepath
        return ''

lang_re = re.compile (r'^@documentlanguage (.+)', re.M)

def extract_sections (filename):
    result = ''
    f = open (filename, 'r')
    page = f.read ()
    f.close()
    # Search document language
    m = lang_re.search (page)
    if m and m.group (1) != 'en':
        lang_suffix = '.' + m.group (1)
    else:
        lang_suffix = ''
    # Replace all includes by their list of sections and extract all sections
    page = include_re.sub (lambda m: expand_includes (m, filename), page)
    sections = section_translation_re.findall (page)
    for sec in sections:
        result += "@" + sec[0] + " " + sec[1] + "\n"
    return (lang_suffix, result)

# Convert a given node name to its proper file name (normalization as explained
# in the texinfo manual:
# http://www.gnu.org/software/texinfo/manual/texinfo/html_node/HTML-Xref-Node-Name-Expansion.html
def texinfo_file_name(title):
    # exception: The top node is always mapped to index.html
    if title == "Top":
        return "index"
    # File name normalization by texinfo (described in the texinfo manual):
    # 1/2: letters and numbers are left unchanged
    # 3/4: multiple, leading and trailing whitespace is removed
    title = title.strip ();
    title = whitespaces.sub (' ', title)
    # 5:   all remaining spaces are converted to '-'
    # 6:   all other 7- or 8-bit chars are replaced by _xxxx (xxxx=ascii character code)
    result = ''
    for index in range(len(title)):
        char = title[index]
        if char == ' ': # space -> '-'
            result += '-'
        elif ( ('0' <= char and char <= '9' ) or
               ('A' <= char and char <= 'Z' ) or
               ('a' <= char and char <= 'z' ) ):  # number or letter
            result += char
        else:
            ccode = ord(char)
            if ccode <= 0xFFFF:
                result += "_%04x" % ccode
            else:
                result += "__%06x" % ccode
    # 7: if name begins with number, prepend 't_g' (so it starts with a letter)
    if (result != '') and (ord(result[0]) in range (ord('0'), ord('9'))):
        result = 't_g' + result
    return result

texinfo_re = re.compile (r'@.*{(.*)}')
def remove_texinfo (title):
    return texinfo_re.sub (r'\1', title)

def create_texinfo_anchor (title):
    return texinfo_file_name (remove_texinfo (title))

unnumbered_re = re.compile (r'unnumbered.*')
def process_sections (filename, lang_suffix, page):
    sections = section_translation_re.findall (page)
    basename = os.path.splitext (os.path.basename (filename))[0]
    p = os.path.join (outdir, basename) + lang_suffix + '.xref-map'
    f = open (p, 'w')

    this_title = ''
    this_filename = 'index'
    this_anchor = ''
    this_unnumbered = False
    had_section = False
    for sec in sections:
        if sec[0] == "node":
            # Write out the cached values to the file and start a new section:
            if this_title != '' and this_title != 'Top':
                    f.write (this_title + "\t" + this_filename + "\t" + this_anchor + "\n")
            had_section = False
            this_title = remove_texinfo (sec[1])
            this_anchor = create_texinfo_anchor (sec[1])
        elif sec[0] == "translationof":
            anchor = create_texinfo_anchor (sec[1])
            # If @translationof is used, it gives the original node name, which
            # we use for the anchor and the file name (if it is a numbered node)
            this_anchor = anchor
            if not this_unnumbered:
                this_filename = anchor
        else:
            # Some pages might not use a node for every section, so treat this
            # case here, too: If we already had a section and encounter enother
            # one before the next @node, we write out the old one and start
            # with the new values
            if had_section and this_title != '':
                f.write (this_title + "\t" + this_filename + "\t" + this_anchor + "\n")
                this_title = remove_texinfo (sec[1])
                this_anchor = create_texinfo_anchor (sec[1])
            had_section = True

            # unnumbered nodes use the previously used file name, only numbered
            # nodes get their own filename! However, top-level @unnumbered
            # still get their own file.
            this_unnumbered = unnumbered_re.match (sec[0])
            if not this_unnumbered or sec[0] == "unnumbered":
                this_filename = this_anchor

    if this_title != '' and this_title != 'Top':
        f.write (this_title + "\t" + this_filename + "\t" + this_anchor + "\n")
    f.close ()


for filename in files:
    print "extract_texi_filenames.py: Processing %s" % filename
    (lang_suffix, sections) = extract_sections (filename)
    process_sections (filename, lang_suffix, sections)
