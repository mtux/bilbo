include_directories( ${KDE4_INCLUDES} )

set(bilbolib_SRCS
 ktexthtmlbuilder.cpp
 kmarkupdirector.cpp
 kmarkupdirector_p.cpp
)

kde4_add_library( bilbomarkupbuilders  STATIC ${bilbolib_SRCS} )