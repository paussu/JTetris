if(NOT DEFINED triplet_dir OR triplet_dir STREQUAL "")
    return()
endif()

if(NOT DEFINED target_dir OR target_dir STREQUAL "")
    message(FATAL_ERROR "target_dir was not provided")
endif()

if(build_config STREQUAL "Debug")
    set(runtime_dir "${triplet_dir}/debug/bin")
else()
    set(runtime_dir "${triplet_dir}/bin")
endif()

if(NOT EXISTS "${runtime_dir}")
    return()
endif()

file(GLOB runtime_dlls "${runtime_dir}/*.dll")

if(runtime_dlls)
    file(COPY ${runtime_dlls} DESTINATION "${target_dir}")
endif()