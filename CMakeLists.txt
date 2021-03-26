project(Vector)

add_executable(
    main test/Source.cpp
    src/Logger.cpp
    src/Logger.h
    src/Vector.cpp
    src/Vector.h
    include/ILogger.h
    include/IVector.h
    include/RC.h
)
