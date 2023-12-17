macro(setResourceInfo RES_FILES)
    add_definitions(-DVER_FILETYPE=VFT_APP)
    add_definitions(-DVER_COMPANYNAME="${VER_COMPANYNAME}")
    add_definitions(-DVER_ORIGINALFILENAME="${VER_ORIGINALFILENAME}")
    add_definitions(-DVER_INTERNALNAME="${VER_INTERNALNAME}")
    add_definitions(-DVER_PRODUCTNAME="${VER_PRODUCTNAME}")
    add_definitions(-DVER_FILEDESCRIPTION="${VER_FILEDESCRIPTION}")


    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(${RES_FILES}
            "${STRATA_DIR}/resource/version.rc"
            )
    else()
      #explicitly build manifest
      set(${RES_FILES}
          "${STRATA_DIR}/resource/version.rc"
          "${STRATA_DIR}/resource/manifest.rc"
            )
    endif()


    if (${CMAKE_RC_COMPILER} MATCHES ".*windres.*")
    #windres cannot handle quoted strings with spaces, so replace them by escape sequence

        if(${VER_COMPANYNAME} MATCHES ".*[ ].*")
            string(REPLACE " " "\\x20" VER_COMPANYNAME_R ${VER_COMPANYNAME})
            set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -UVER_COMPANYNAME -DVER_COMPANYNAME=${VER_COMPANYNAME_R}")
        endif()

        if(${VER_ORIGINALFILENAME} MATCHES ".*[ ].*")
            string(REPLACE " " "\\x20" VER_ORIGINALFILENAME_R ${VER_ORIGINALFILENAME})
            set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -UVER_ORIGINALFILENAME -DVER_ORIGINALFILENAME=${VER_ORIGINALFILENAME_R}")
        endif()

        if(${VER_INTERNALNAME} MATCHES ".*[ ].*")
            string(REPLACE " " "\\x20" VER_INTERNALNAME_R ${VER_INTERNALNAME})
            set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -UVER_INTERNALNAME -DVER_INTERNALNAME=${VER_INTERNALNAME_R}")
        endif()

        if(${VER_PRODUCTNAME} MATCHES ".*[ ].*")
            string(REPLACE " " "\\x20" VER_PRODUCTNAME_R ${VER_PRODUCTNAME})
            set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -UVER_PRODUCTNAME -DVER_PRODUCTNAME=${VER_PRODUCTNAME_R}")
        endif()

        if(${VER_FILEDESCRIPTION} MATCHES ".*[ ].*")
            string(REPLACE " " "\\x20" VER_FILEDESCRIPTION_R ${VER_FILEDESCRIPTION})
            set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -UVER_FILEDESCRIPTION -DVER_FILEDESCRIPTION=${VER_FILEDESCRIPTION_R}")
        endif()

    endif()
endmacro()
