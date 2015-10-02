if (ENABLE_TESTING)
  add_subdirectory (tests)
endif (ENABLE_TESTING)

option (TRACE_THREADING "Trace threading execution" OFF)
option (RANGE_CHECKING "Enable run-time checks of array indexing" OFF)

# Note: The following add_definitions statements actually need to come in
# any project that uses bonelab, because the affected files are included with
# include statements.

if (TRACE_THREADING)
  add_definitions (-DTRACE_THREADING)
endif (TRACE_THREADING)

if (RANGE_CHECKING)
  add_definitions (-DRANGE_CHECKING)
endif (RANGE_CHECKING)
