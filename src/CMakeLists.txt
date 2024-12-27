add_library(${lib_name}
        ${lib_name}/ili9341_draw.c
        ${lib_name}/ili9341_draw.h
        ${lib_name}/ili9341_framebuffer.c
        ${lib_name}/ili9341_framebuffer.h
        ${lib_name}/ili9341.c
        ${lib_name}/ili9341.h
        ${lib_name}/ugui.c
        ${lib_name}/ugui.h
        ${lib_name}/xpt2046.c
        ${lib_name}/xpt2046.h
        )

target_link_libraries(${lib_name}
        hardware_spi
        #hardware_timer
)