# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./plugins/Engine/Engine/InteractionEngine/NarrowInteractionGeometryEngine/SwiftPolyhedronProximityModeler
# Target is a library:  

LIBS += -lyade-lib-swiftpp \
        -lPolyhedralSweptSphere \
        -lMacroMicroContactGeometry \
        -rdynamic 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../../../../libraries/yade-lib-swiftpp/$(YADEDYNLIBPATH)/yade-libs \
               ../../../../../../plugins/Data/Body/InteractingGeometry/PolyhedralSweptSphere/$(YADEDYNLIBPATH) \
               $(YADEDYNLIBPATH)/yade-libs \
               $(YADEDYNLIBPATH) 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = $(YADEDYNLIBPATH) 
CONFIG += debug \
          warn_on \
          dll 
TEMPLATE = lib 
HEADERS += SwiftPolyhedronProximityModeler.hpp 
SOURCES += SwiftPolyhedronProximityModeler.cpp 