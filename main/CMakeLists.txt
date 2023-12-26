idf_component_register(
    SRCS
    "main.c"
    "deauth.c"
    INCLUDE_DIRS
    "."
)
target_link_libraries(${COMPONENT_LIB} -Wl,-zmuldefs)
