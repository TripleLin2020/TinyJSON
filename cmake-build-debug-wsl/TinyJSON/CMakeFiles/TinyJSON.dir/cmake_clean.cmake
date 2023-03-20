file(REMOVE_RECURSE
  "../lib/libTinyJSON.a"
  "../lib/libTinyJSON.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/TinyJSON.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
