#!./DEBUG/server

service_create time

# Create object references
set zimbabwe [object_activate time Zimbabwe]
set haiti [object_activate time Haiti]
set gambia [object_activate time Gambia]

# Write object references to file.
# Only need this if were not using the Naming Service
#set fooior [ open /tmp/foo.ior w ]
#set barior [ open /tmp/bar.ior w ]
#puts $fooior $foo
#puts $barior $bar
#close $fooior
#close $barior

# Bind object references in Naming Service
name_rebind Zimbabwe $zimbabwe
name_rebind Haiti $haiti
name_rebind Gambia $gambia

# Activate the service
service_set_state time ACTIVE

# If the script terminates, the process dies and the ORB with it
# so we wait on an undefined variable
vwait forever