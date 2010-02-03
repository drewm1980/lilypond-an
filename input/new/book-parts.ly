\version "2.11.65"

\header {
  lsrtags = "paper-and-layout"
  texidoc = "@code{\\bookpart} can be used to split a book into
several parts.
Each part last page can be affected by @code{ragged-bottom-last}.
Header and footer markups can detect a part last page, and make the
difference with the book last page.
"
  doctitle = "Book parts"
}

#(set-default-paper-size "a6")

\book {

  %% book paper, which is inherited by all children bookparts
  \paper {
    ragged-last-bottom = ##t
    %% Page footer: add a different part-tagline at part last page
    oddFooterMarkup = \markup {
      \column {
        \fill-line {
          %% Copyright header field only on book first page.
          \on-the-fly #first-page \fromproperty #'header:copyright
        }
        \fill-line {
          %% Part tagline header field only on each part last page.
          \on-the-fly #part-last-page \fromproperty #'header:parttagline
        }
        \fill-line {
          %% Tagline header field only on book last page.
          \on-the-fly #last-page \fromproperty #'header:tagline
        }
      }
    }
  }

  %% book header, which is inherited by the first bookpart
  \header {
    title = "Book title"
    copyright = "Copyright line on book first page"
    parttagline = "Part tagline"
    tagline = "Book tagline"
  }
  
  \bookpart {
    %% a different page breaking function may be used on each part
    \paper { #(define page-breaking optimal-page-breaks) }
    \header { subtitle = "First part" }
    \markup { The first book part }
    \markup { a page break }
    \pageBreak
    \markup { first part last page }
    \markup \wordwrap { with ragged-last-bottom (see the space below this text) }
  }

  \bookpart {
    \header { subtitle = "Second part" }
    { c' }
  }

}
