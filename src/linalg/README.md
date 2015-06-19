liblinalg
============

Simple Linear Algebra Routines

Implementation details
----------------------

* General
  * The programming is simple C. This helps in keeping it cache-efficient and compile-time quick.

Build steps
-----------

liblibalg is built using autotools:

    $ ./autogen.sh
    $ ./configure
    $ make
    $ sudo make install  # optional
