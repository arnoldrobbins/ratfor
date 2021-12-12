# RATFOR --- Rational Fortran

This repository tracks the history of the original C version of
the Bell Labs `ratfor` translator written by Brian Kernighan.
It include the original versions from the Sixth, Seventh, Eighth
and Tenth Edition UNIX(R) systems.  There is a separate branch with
each version of the original code. (The Ninth Edition sources in
the TUHS archives did not have ratfor.)

In addition, starting with the version from the Tenth Edition, I have
modernized the code to compile and run on a current day GNU/Linux
system. It undoubtedly needs more work.  This is what you see in the
master branch.

This version requires Berkeley Yacc (`byacc`) to be available. Its
use of `yyval` precludes the use of Bison, where `yyval` is not a
globally available variable.

Source code is in the `src` directory. The manual page is in the `man`
directory.  Documentation is in the `doc` directory.  You can format
the documentation with:

	cd macros
	groff -C tmac.s ../doc/m.mac ../doc/m[0-9]* | ps2pdf - /tmp/ratfor.pdf

The documentation is that of the Seventh Edition system; it was not
in the V8 tape or the V10 tape.

The manual page can be formatted as on a modern system:

	groff -man ratfor.1 | ps2pdf - /tmp/ratfor.1.pdf

I have not (yet) renamed any of the files. In particular, the use of
a `.g` suffix for the grammar instead of the standard `.y` is
rather quaint.

Enjoy,

Arnold Robbins

#### Last Updated

Sun Dec 12 20:44:40 IST 2021
