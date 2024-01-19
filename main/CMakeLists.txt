idf_component_register(
    SRCS
    "main.c"
    "main_deauth.c"
    "deauth.c"
    "sniff.c"
    "serializer.c"
    "http.c"
    "softap_sta.c"
    INCLUDE_DIRS
    "."
)
target_link_libraries(${COMPONENT_LIB} -Wl,-zmuldefs)
