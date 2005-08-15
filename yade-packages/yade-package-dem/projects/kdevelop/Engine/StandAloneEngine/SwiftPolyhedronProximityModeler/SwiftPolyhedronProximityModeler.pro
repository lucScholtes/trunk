# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./Engine/StandAloneEngine/SwiftPolyhedronProximityModeler
# Target is a library:  

LIBS += -lMacroMicroContactGeometry \
        -lPolyhedralSweptSphere \
        -lyade-lib-swiftpp \
        -rdynamic 
INCLUDEPATH += ../../../DataClass/InteractionGeometry/MacroMicroContactGeometry \
               ../../../DataClass/InteractingGeometry/PolyhedralSweptSphere 
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
HEADERS += SwiftPolyhedronProximityModeler.hpp 
SOURCES += SwiftPolyhedronProximityModeler.cpp 