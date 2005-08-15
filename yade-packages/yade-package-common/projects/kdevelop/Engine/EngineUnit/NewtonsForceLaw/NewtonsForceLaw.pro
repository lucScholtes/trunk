# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./Engine/EngineUnit/NewtonsForceLaw
# Target is a library:  

LIBS += -lPhysicalActionApplier \
        -lForce \
        -lParticleParameters \
        -lPhysicalActionApplier \
        -rdynamic 
INCLUDEPATH += ../../../DataClass/PhysicalAction/Force \
               ../../../DataClass/PhysicalParameters/ParticleParameters \
               ../../../Engine/MetaEngine/PhysicalActionApplier 
QMAKE_LIBDIR = ../../../../../bin \
               ../../../../../bin \
               ../../../../../bin \
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
HEADERS += NewtonsForceLaw.hpp 
SOURCES += NewtonsForceLaw.cpp 