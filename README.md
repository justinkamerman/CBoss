CBoss
=====

A prototype CORBA application server wherby CORBA object behaviour
policies can be implemented as standalone modules and deployment
characteristics defined at runtime without recompilation.

A embedded Tcl/Tk script interpreter exposes POA functions and
provides the primary event loop. Implementation is in C++, using the
TAO ORB: <http://www.cs.wustl.edu/~schmidt/TAO.html>.




![ApplicationServerEventLool](https://raw.github.com/justinkamerman/CBoss/master/images/ApplicationServerEventLoop.jpg)
