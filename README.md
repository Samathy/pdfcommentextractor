pdfcommentextractor
-------------------

Does what it says on the tin. Extracts highlighted text from PDF documents.

#### Dependencies

glib
poppler-glib

#### Build
Run:
    make all

#### Usage

    pdfcommentextractor [-pi] -u [linewidth] -P [pages to extract] -o [output file] -f [input.pdf]
    -p    Show page numbers
    -i    Interactive mode
    -u    Unwrap and de-hyphenate text. Wrap to given linewidth [default=80]

#### TODO

- [x] Extract highlighted text
- [x] Allow interactive editing of rectangles
- [x] Nicely format extracted text
- [x] Extract to stdout or text file
- [] Extract commented text (From floating annotations)

#### Known issues

Due to the awful way highlights in PDFs[1] are dealt with, your milage may vary
on the actual text which gets extracted.
Depending on the layout of the document and where your highlights are, you might get 
more, or less text that you expected.
Try using the interactive mode to edit the rectangle coordinates.

[1] Highlights are annotations on the visual layer of the document.
A highlight has no attachment to the text its highlighting.
 

