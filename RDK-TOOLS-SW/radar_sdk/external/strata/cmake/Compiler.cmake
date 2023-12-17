
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_definitions(-DNOMINMAX) # prevent min/max macros to be defined
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4800") #suppress stupid bool conversion MSVC compiler warning
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996") #suppress misleading deprecated warning for unchecked iterators
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4250") #suppress inheritance via dominance warning for virtual inheritance
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w34263")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w34266")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-multichar") #suppress multi-character character warning, used for endian detection
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override") #warn about missing override specifier
    endif()
endif()
