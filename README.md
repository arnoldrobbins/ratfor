# RATFOR --- Rational Fortran

This repository tracks the history of the original C version of
the Bell Labs `ratfor` translator written by Brian Kernighan.

In this branch you have the code from the Seventh Edition UNIX(tm) system.
Source code is in the `src` directory The manual page is in the `man`
directory.  Documentation is in the `doc` directory.  You can format
the documentation with:

	cd macros
	groff -C tmac.s ../doc/m.mac ../doc/m[0-9]* | ps2pdf - /tmp/ratfor.pdf

I hope to take the latest version (from the Tenth Edition UNIX system)
and modernize it to run on current systems. We'll see if I succeed.

Arnold Robbins

#### Last Updated

Sun Nov 14 21:31:48 IST 2021
