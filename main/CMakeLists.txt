idf_component_register(SRCS "main.c" "main_deauth.c" "sniff.c" "serializer.c" "deauth.c" "http_server.c" "softap_sta.c"
                    INCLUDE_DIRS ".")

                    target_link_libraries(${COMPONENT_LIB} -Wl,-zmuldefs)