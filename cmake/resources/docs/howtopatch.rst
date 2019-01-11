Making patches
--------------

This you need to do when your tarballs need tweaking after they're
unpacked.  Here's a good link on making patches:
http://www.cpqlinux.com/patch.html

Example:  making a patch for boost 1.36.0::

  sudo /opt/i3/ports/bin/port -vd extract boost_1.36.0
  DEBUG: Found port in file:///opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0
  DEBUG: Changing to port directory: /opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0
  DEBUG: Requested variant linux is not provided by port boost_1.36.0.
  DEBUG: Requested variant i686 is not provided by port boost_1.36.0.
  DEBUG: Skipping com.apple.main (boost_1.36.0) since this port is already installed
  DEBUG: Skipping com.apple.fetch (boost_1.36.0) since this port is already installed
  DEBUG: Skipping com.apple.checksum (boost_1.36.0) since this port is already installed
  DEBUG: Skipping com.apple.extract (boost_1.36.0) since this port is already installed
  troy@uranium:~/chasebug
  % sudo /opt/i3/ports/bin/port -vdf extract boost_1.36.0
  DEBUG: Found port in file:///opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0
  DEBUG: Changing to port directory: /opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0
  DEBUG: Requested variant linux is not provided by port boost_1.36.0.
  DEBUG: Requested variant i686 is not provided by port boost_1.36.0.
  DEBUG: Executing com.apple.main (boost_1.36.0)
  --->  Fetching boost_1.36.0
  DEBUG: Executing com.apple.fetch (boost_1.36.0)
  --->  Verifying checksum(s) for boost_1.36.0
  DEBUG: Executing com.apple.checksum (boost_1.36.0)
  --->  Checksumming boost_1_36_0.tar.gz
  DEBUG: Correct (md5) checksum for boost_1_36_0.tar.gz
  --->  Extracting boost_1.36.0
  DEBUG: Executing com.apple.extract (boost_1.36.0)
  --->  Extracting boost_1_36_0.tar.gz
  DEBUG: setting option extract.args to /opt/i3/ports/var/db/dports/distfiles/boost_1.36.0/boost_1_36_0.tar.gz
  DEBUG: Assembled command: 'cd "/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work" && gzip -dc /opt/i3/ports/var/db/dports/distfiles/boost_1.36.0/boost_1_36_0.tar.gz | tar --no-same-owner -xf -'
  
You can see the path it was unpacked to, above.  It is the ``-vd``
flags that get you all of this detailed output.

::

  % cd /opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work                                 
  troy@uranium:/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work
  % ls
  boost_1.36.0/
  troy@uranium:/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work
  % cd boost_1.36.0/libs/serialization 
  troy@uranium:/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/serialization
  % ls
  CMakeLists.txt  build/  doc/  example/  index.html  module.cmake  performance/  src/  test/  util/  vc7ide/
  troy@uranium:/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/serialization
  % cd src
  troy@uranium:/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/serialization/src
  % ls
  CMakeLists.txt         basic_pointer_iserializer.cpp  basic_text_woprimitive.cpp  binary_woarchive.cpp            polymorphic_oarchive.cpp  utf8_codecvt_facet.cpp  xml_wiarchive.cpp
  basic_archive.cpp      basic_pointer_oserializer.cpp  basic_xml_archive.cpp       codecvt_null.cpp                stl_port.cpp              void_cast.cpp           xml_woarchive.cpp
  basic_iarchive.cpp     basic_serializer_map.cpp       basic_xml_grammar.ipp       extended_type_info.cpp          text_iarchive.cpp         xml_grammar.cpp
  basic_iserializer.cpp  basic_text_iprimitive.cpp      binary_iarchive.cpp         extended_type_info_no_rtti.cpp  text_oarchive.cpp         xml_iarchive.cpp
  basic_oarchive.cpp     basic_text_oprimitive.cpp      binary_oarchive.cpp         extended_type_info_typeid.cpp   text_wiarchive.cpp        xml_oarchive.cpp
  basic_oserializer.cpp  basic_text_wiprimitive.cpp     binary_wiarchive.cpp        polymorphic_iarchive.cpp        text_woarchive.cpp        xml_wgrammar.cpp
  troy@uranium:/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/serialization/src
  %    
  
copy the files you want to modify to *filename*.orig and modify the non-.orig version::

  cp extended_type_info.cpp extended_type_info.cpp.orig
  emacs extended_type_info.cpp

Then make your patchfile, putting the .orig first::

  diff -Naur extended_type_info.cpp.org extended_type_info.cpp > /tmp/extended_type_info_patch

The patchfile might look like this::

  % cat /tmp/extended_type_info_patch    
  --- extended_type_info.cpp.orig	2008-10-09 10:07:17.000000000 -0400
  +++ extended_type_info.cpp	2008-10-09 10:09:25.000000000 -0400
  @@ -58,7 +58,19 @@
       }
   };
   
  -typedef std::multiset<const extended_type_info *, key_compare> ktmap;
  +struct ktmap : 
  +  public std::multiset<const extended_type_info *, key_compare>
  +{
  +  static bool m_destroyed;
  +  static bool is_destroyed(){
  +    return m_destroyed;
  +  }
  +  ~ktmap(){
  +    m_destroyed = true;
  +  }
  +};
  +
  +bool ktmap::m_destroyed = false;
   
Modify the paths in the patchfile to point to the same file (the non-.orig version)
and make the path relative to the root of the unpacked tarball::

  --- libs/serialization/src/extended_type_info.cpp	2008-10-09 10:07:17.000000000 -0400
  +++ libs/serialization/src/extended_type_info.cpp	2008-10-09 10:09:25.000000000 -0400
  (etc)

Copy the patchfile to the ``files/`` directory of the port and add them to the ``Portfile``::

  cp /tmp/extended_type_info_patch /path/to/ports/devel/boost_1.36.0/files
  emacs /path/to/ports/checkout/devel/boost_1.36.0/Portfile

The Portfile line would look like::

  patchfiles          extended_type_info_patch

Increment the **revision** in the portfile::

  revision 7

When you've checked everything in, test again on your local copy.
First sync your ports::

  port -vd sync

You should see the update go by::

  devel/boost_1.36.0/
  devel/boost_1.36.0/Portfile
  devel/boost_1.36.0/files/
  devel/boost_1.36.0/files/extended_type_info_patch
  devel/boost_1.36.0/files/extended_type_info_typeid_patch

Now have ports unpack and patch the file::

  % sudo /opt/i3/ports/bin/port -vd patch boost_1.36.0
  DEBUG: Found port in file:///opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0
  DEBUG: Changing to port directory: /opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0
  DEBUG: Requested variant linux is not provided by port boost_1.36.0.
  DEBUG: Requested variant i686 is not provided by port boost_1.36.0.
  DEBUG: Skipping completed com.apple.main (boost_1.36.0)
  DEBUG: Skipping completed com.apple.fetch (boost_1.36.0)
  DEBUG: Skipping completed com.apple.checksum (boost_1.36.0)
  --->  Extracting boost_1.36.0
  DEBUG: Executing com.apple.extract (boost_1.36.0)
  --->  Extracting boost_1_36_0.tar.gz
  DEBUG: setting option extract.args to /opt/i3/ports/var/db/dports/distfiles/boost_1.36.0/boost_1_36_0.tar.gz
  DEBUG: Assembled command: 'cd "/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work" && gzip -dc /opt/i3/ports/var/db/dports/distfiles/boost_1.36.0/boost_1_36_0.tar.gz | tar --no-same-owner -xf -'
  DEBUG: Executing com.apple.patch (boost_1.36.0)
  --->  Applying patches to boost_1.36.0
  --->  Applying /opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0/files/extended_type_info_patch
  DEBUG: Assembled command: 'cd "/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0" && patch -p0'
  patching file libs/serialization/src/extended_type_info.cpp
  --->  Applying /opt/i3/ports/var/db/dports/sources/rsync.code.icecube.wisc.edu_icecube-tools-ports/devel/boost_1.36.0/files/extended_type_info_typeid_patch
  DEBUG: Assembled command: 'cd "/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0" && patch -p0'
  patching file libs/serialization/src/extended_type_info_typeid.cpp

Verify that there are no errors from the patch program.  Using the
paths in the output above, go have a look at your patched files::

  % cat "/opt/i3/ports/var/db/dports/build/file._opt_i3_ports_var_db_dports_sources_rsync.code.icecube.wisc.edu_icecube-tools-ports_devel_boost_1.36.0/work/boost_1.36.0/libs/serialization/src/extended_type_info.cpp"
  (verify that the thing looks like you want it)

Then you can just install your port::

  % port -vd install boost_1.36.0
