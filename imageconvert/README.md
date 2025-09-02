## Image Convert Tool
This directory contains tools to convert between Windows BMP, PNG and my internal format (Group5).

## What is Group5?
My idea for the Group5 image compression came from CCITT Group 4 (or G4 for short). G4 is a lossless
two-dimensional image compression invented to compress digital facsimile (FAX) images. It's a very
effective compression that's fast and not very complicated. It treats the page as a series of black
and white runs of pixels and then applies a statistical (Huffman) compression to them. The ues case
I had in mind for this image compression is very constrained microprocessors. The only problem is
that the Huffman tables for fast decoding require about 8K of ROM (using my single lookup method).
For a tiny MCU this might be too large of a space to sacrifice for a lookup table. I thought of
a compromise which loses a small amount of compression effectiveness in return for simplification
of the decoder and the elimination of the lookup table. It only takes about 250 lines of C code
to implement the G5 decoder. Since this idea is derived from G4, I named it Group5.

## What is the benefit?
With G5 image compression, my imaging libraries offer a very efficient way to work with fonts and
2D images that can run on very humble target hardware. This allows for smaller programs and data
sets, compresses at up to 20 to 1 and is fast to decompress. In many cases, G5 performs better
than PNG compression and requires much less code to implement. The encoder and decoder are
written in portable C code that should run on almost any CPU available. In fact, it's 
currently used in the openepaperlink project and runs on the many decades old, 8-bit 8051 MCUs
inside of inexpensive e-paper price tags.

## Convenient tools
In order to help with the adoption and usage of G5, I needed to create a set of tools that
make it easy to convert to and from this format.

## Going further
The basic G5 compression only supports 1-bit pixels. Three and four color e-paper displays
have become more common in recent years and I wanted to create a solution to address them
as well. 
