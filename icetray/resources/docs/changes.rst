Version 3
=========

* I3Tray considers the first module added to be the 'driving' module.
  This should be a reader or generator, ie a module that does not
  access its outbox.  In the past the semantics of this were a little
  fuzzier as modules had a half-implemented scheme whereby they could add
  inboxes.  (At some point it was decided that modules may have one and
  only one inbox).





