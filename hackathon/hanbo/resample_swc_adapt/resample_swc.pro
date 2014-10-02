
TEMPLATE	= lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
VAA3DPATH = D:\vaa3D\v3d_external
INCLUDEPATH	+= $$VAA3DPATH/v3d_main/basic_c_fun

HEADERS	+= resample_swc_func.h
HEADERS	+= resample_swc_plugin.h
SOURCES	+= resample_swc_func.cpp
SOURCES	+= resample_swc_plugin.cpp
SOURCES	+= $$VAA3DPATH/v3d_main/basic_c_fun/v3d_message.cpp
SOURCES	+= $$VAA3DPATH/v3d_main/basic_c_fun/basic_surf_objs.cpp

TARGET	= $$qtLibraryTarget(resample_swc)
DESTDIR	= $$VAA3DPATH/bin/plugins/resample_swc/
