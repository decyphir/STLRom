add_library(robonTL.a STATIC ${})

install (TARGETS robonTL.a DESTINATION lib)

file(GLOB headers *.h *.hh)

install(FILES ${headers} DESTINATION include/${pyrobonTL})
