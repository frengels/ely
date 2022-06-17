set(DRIVER_SRC
    "driver.cpp")

add_executable(elyc ${DRIVER_SRC})
target_link_libraries(elyc PRIVATE ely)