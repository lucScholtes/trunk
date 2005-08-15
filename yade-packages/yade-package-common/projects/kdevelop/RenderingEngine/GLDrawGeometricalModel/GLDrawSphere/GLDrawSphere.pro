# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./RenderingEngine/GLDrawGeometricalModel/GLDrawSphere
# Target is a library:  

LIBS += -lSphere \
        -lyade-lib-opengl \
        -rdynamic 
INCLUDEPATH += ../../../DataClass/GeometricalModel/Sphere \
               ../../../RenderingEngine/OpenGLRenderingEngine 
QMAKE_LIBDIR = ../../../../../bin \
               /usr/local/lib/yade/yade-libs/ 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = ../../../../../bin 
CONFIG += debug \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += GLDrawSphere.hpp 
SOURCES += GLDrawSphere.cpp 