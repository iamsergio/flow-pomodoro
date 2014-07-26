*-g++*|*clang* {
    QMAKE_CXXFLAGS = -Werror -Wall -Wextra
}


INCLUDEPATH += src

CONFIG += debug c++11
