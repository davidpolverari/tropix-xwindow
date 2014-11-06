Herein lies the (partial?) list of Known Bugs.  Further bug reports
(or even better, solutions) can be sent to the FVWM mailing list (see
the FAQ for address and how to PROPERLY report bugs).

See 'Bugfixes' section of the TO-DO list for additional known bugs.
There are a number of them there as well.

======================================================================
======================================================================

Binding a key to a window decoration but not to the window itself is
discouraged because when the key-press event finally gets to the
window it will be marked as SYNTHETIC and will be ignored by many
applications.

----------------------------------------------------------------------

Sending DESTROY window manager options to applications is a bad way to
close them and should only be used as a last resort.  Strange things
can happen.  Please try DELETE and CLOSE first.

----------------------------------------------------------------------

Some users have seen intermittent problems with XEmacs version 19.13
not being refreshed correctly after a restart of fvwm2.  If this
occurs, you should be able to open a new frame and delete the old one.
I can't debug this, as I don't see this problem.  I don't know if the
problem is from XEmacs, fvwm2, or something specific to a few user's
setups.

----------------------------------------------------------------------

Some users have been getting odd startup problems, like total lockups.
I cannot reproduce this, and have no idea why.  Let me know if you see
this and find a way to consistently reproduce it.

Actually, I've recently discovered that this is often from not having
a .fvwm2rc file, so check that first...

----------------------------------------------------------------------

The FvwmConfig module doesn't get built by default.  You have to build
it by hand (see INSTALL file).  It now has an Imakefile, but can only
be used under X11R6.

----------------------------------------------------------------------


