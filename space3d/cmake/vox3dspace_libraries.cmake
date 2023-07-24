
# Vox3DSpace libraries building process.

macro(build_libraries)
  
  vox3dspace_add_library(NAME vox3dspace_common_base TYPE OBJECT SOURCES
                    ${vox3dspace_common_base_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_common_math TYPE OBJECT SOURCES
                    ${vox3dspace_common_math_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    LIB_DEPS
                    ${vox3dspace_common_base}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_data_stream TYPE OBJECT SOURCES
                    ${vox3dspace_engine_data_stream_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_data_ctm TYPE OBJECT SOURCES
                    ${vox3dspace_engine_data_ctm_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_math TYPE OBJECT SOURCES
                    ${vox3dspace_engine_math_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_text TYPE OBJECT SOURCES
                    ${vox3dspace_engine_text_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_engine_view TYPE OBJECT SOURCES
                    ${vox3dspace_engine_view_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  vox3dspace_add_library(NAME vox3dspace_profile TYPE SHARED SOURCES
                    ${vox3dspace_profile_sources}
                    DEFINES
                    ${vox3dspace_defines}
                    )
  # vox3dspace_add_library(NAME vox3dspace_engine_base TYPE OBJECT SOURCES
  #                   ${vox3dspace_engine_base_src}
  #                   DEFINES
  #                   ${vox3dspace_defines}
  #                   )

  
  # list(APPEND vox3dspace_engine_library_deps
  #   vox3dspace_engine_base
  #   vox3dspace_common_math
  #   )
  list(APPEND vox3dspace_engine_library_deps
    vox3dspace_engine_data_stream
    vox3dspace_engine_math
    vox3dspace_common_math
    vox3dspace_engine_text
    vox3dspace_engine_view
    )
  list(APPEND vox3dspace_ctm_library_deps
    vox3dspace_engine_data_stream
    vox3dspace_engine_data_ctm
    )
endmacro()