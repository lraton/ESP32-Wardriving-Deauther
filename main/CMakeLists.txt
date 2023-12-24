idf_component_register(
    SRCS
    "main.cpp"
    "deauth.cpp"
    INCLUDE_DIRS
    "."
)
target_link_libraries(${COMPONENT_LIB} -Wl,-zmuldefs)
