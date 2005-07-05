# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./plugins/Engine/EngineUnit/BodyEngineUnit/BoundingVolumeEngineUnit/Box2AABB
# Target is a library:  

LIBS += -lBoundingVolumeMetaEngine \
        -lyade-lib-wm3-math \
        -lyade-lib-multimethods \
        -lInteractingBox \
        -lAABB \
        -lBox \
        -rdynamic 
INCLUDEPATH += $(YADEINCLUDEPATH) 
MOC_DIR = $(YADECOMPILATIONPATH) 
UI_DIR = $(YADECOMPILATIONPATH) 
OBJECTS_DIR = $(YADECOMPILATIONPATH) 
QMAKE_LIBDIR = ../../../../../../plugins/Engine/MetaEngine/BodyMetaEngine/BoundingVolumeMetaEngine/BoundingVolumeMetaEngine/$(YADEDYNLIBPATH) \
               /usr/local/lib/yade/yade-libs/ \
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
HEADERS += Box2AABB.hpp 
SOURCES += Box2AABB.cpp 