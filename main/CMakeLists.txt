set(srcs "main.c"
        "board.c"
        "led_config.c"
        "app.c")

idf_component_register(SRCS "bluetooth_app.c" "${srcs}"
                    INCLUDE_DIRS  ".")
