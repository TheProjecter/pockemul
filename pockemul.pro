CONFIG +=   build_all \
  debug_and_release \
  qt \
  resources \
  thread \
  warn_on
FORMS +=   ui/about.ui \
  ui/dialoganalog.ui \
  ui/dialogconnectorlink.ui \
  ui/dialogdump.ui \
  ui/dialogkeylist.ui \
  ui/dialoglog.ui \
  ui/pockemul.ui \
  ui/serialconsole.ui \
  ui/startup.ui
HEADERS +=   src/core/Connect.h \
  src/core/Dasm.h \
  src/core/Debug.h \
  src/core/Inter.h \
  src/core/Keyb.h \
  src/core/Keyb1250.h \
  src/core/Keyb1251.h \
  src/core/Keyb1350.h \
  src/core/Keyb1360.h \
  src/core/Keyb1401.h \
  src/core/Keyb1403.h \
  src/core/Keyb1450.h \
  src/core/Keyb1500.h \
  src/core/Lcdc.h \
  src/core/Lcdc_symb.h \
  src/core/Log.h \
  src/core/analog.h \
  src/core/clink.h \
  src/core/common.h \
  src/core/cprinter.h \
  src/core/dialogabout.h \
  src/core/dialoganalog.h \
  src/core/dialogconsole.h \
  src/core/dialogdump.h \
  src/core/dialogkeylist.h \
  src/core/dialoglog.h \
  src/core/dialogstartup.h \
  src/core/extension.h \
  src/core/global.h \
  src/core/hexviewer.h \
  src/core/init.h \
  src/core/keybce150.h \
  src/core/keybce152.h \
  src/core/mainwindowpockemul.h \
  src/core/paperwidget.h \
  src/core/pobject.h \
  src/core/slot.h \
  src/core/xmlwriter.h \
  src/cpu/cpu.h \
  src/cpu/lh5801.h \
  src/cpu/lh5810.h \
  src/cpu/pd1990ac.h \
  src/cpu/sc61860.h \
  src/machine/Ce126.h \
  src/machine/ccable.h \
  src/machine/ce125.h \
  src/machine/ce150.h \
  src/machine/ce152.h \
  src/machine/pc1245.h \
  src/machine/pc1250.h \
  src/machine/pc1251.h \
  src/machine/pc1255.h \
  src/machine/pc1260.h \
  src/machine/pc1350.h \
  src/machine/pc1360.h \
  src/machine/pc1401.h \
  src/machine/pc1402.h \
  src/machine/pc1403.h \
  src/machine/pc1450.h \
  src/machine/pc1475.h \
  src/machine/pc1500.h \
  src/machine/pcxxxx.h \
  src/machine/sio.h \
  src/core/lfhex/translate.h \
  src/core/lfhex/box.h \
  src/core/lfhex/compareDlg.h \
  src/core/lfhex/conversionDlg.h \
  src/core/lfhex/cursor.h \
  src/core/lfhex/delta.h \
  src/core/lfhex/expr.h \
  src/core/lfhex/grid.h \
  src/core/lfhex/hexEditor.h \
  src/core/lfhex/hexGui.h \
  src/core/lfhex/local.h \
  src/core/lfhex/mappings.h \
  src/core/lfhex/offsetConstraint.h \
  src/core/lfhex/reader.h \
  src/core/lfhex/save.h
INCLUDEPATH +=   . \
  src/core \
  src/cpu \
  src/machine \
  R:/fmodapi375win/api/inc
LANGUAGE +=   C++
LIBS +=   R:/fmodapi375win/api/lib/libfmod.a
LICENSE +=   GPL
MOC_DIR +=   build/moc
OPENEDFILES += 
QT +=   core \
  gui \
  opengl \
  xml
QTPLUGIN +=   qjpeg
RCC_DIR +=   build/rcc
RESOURCES +=   resources/ext.qrc \
  resources/pc1245.qrc \
  resources/pc1250.qrc \
  resources/pc1251.qrc \
  resources/pc1255.qrc \
  resources/pc1260.qrc \
  resources/pc1261.qrc \
  resources/pc1262.qrc \
  resources/pc1350.qrc \
  resources/pc1360.qrc \
  resources/pc1401.qrc \
  resources/pc1402.qrc \
  resources/pc1403.qrc \
  resources/pc1450.qrc \
  resources/pc1475.qrc \
  resources/pc1500.qrc \
  resources/pockemul.qrc \
  src/core/lfhex/lfhex.qrc
SOURCES +=   src/core/Connect.cpp \
  src/core/Dasm.cpp \
  src/core/Debug.cpp \
  src/core/Inter.cpp \
  src/core/Keyb.cpp \
  src/core/Lcdc.cpp \
  src/core/Log.cpp \
  src/core/analog.cpp \
  src/core/clink.cpp \
  src/core/cprinter.cpp \
  src/core/dialogabout.cpp \
  src/core/dialoganalog.cpp \
  src/core/dialogconsole.cpp \
  src/core/dialogdump.cpp \
  src/core/dialogkeylist.cpp \
  src/core/dialoglog.cpp \
  src/core/dialogstartup.cpp \
  src/core/extension.cpp \
  src/core/hexviewer.cpp \
  src/core/init.cpp \
  src/core/mainwindowpockemul.cpp \
  src/core/paperwidget.cpp \
  src/core/pcxxxx_ext.cpp \
  src/core/pobject.cpp \
  src/core/pockemul.cpp \
  src/core/slot.cpp \
  src/core/xmlwriter.cpp \
  src/cpu/cpu.cpp \
  src/cpu/lh5801.cpp \
  src/cpu/lh5810.cpp \
  src/cpu/pd1990ac.cpp \
  src/cpu/sc61860.cpp \
  src/machine/Ce126.cpp \
  src/machine/ccable.cpp \
  src/machine/ce125.cpp \
  src/machine/ce150.cpp \
  src/machine/ce152.cpp \
  src/machine/pc1245.cpp \
  src/machine/pc1250.cpp \
  src/machine/pc1251.cpp \
  src/machine/pc1255.cpp \
  src/machine/pc1260.cpp \
  src/machine/pc1350.cpp \
  src/machine/pc1360.cpp \
  src/machine/pc1401.cpp \
  src/machine/pc1402.cpp \
  src/machine/pc1403.cpp \
  src/machine/pc1450.cpp \
  src/machine/pc1475.cpp \
  src/machine/pc1500.cpp \
  src/machine/pcxxxx.cpp \
  src/machine/sio.cpp \
  src/core/lfhex/compareDlg.cpp \
  src/core/lfhex/conversionDlg.cpp \
  src/core/lfhex/cursor.cpp \
  src/core/lfhex/delta.cpp \
  src/core/lfhex/driver.cpp \
  src/core/lfhex/grid.cpp \
  src/core/lfhex/hexEditor.cpp \
  src/core/lfhex/hexGui.cpp \
  src/core/lfhex/offsetConstraint.cpp \
  src/core/lfhex/reader.cpp \
  src/core/lfhex/save.cpp \
  src/core/lfhex/translate.cpp
TARGET =   Pockemul
TEMPLATE =   app
TRANSLATIONS +=   pockemul_fr.ts
UI_DIR +=   build/ui
mac {
  OBJECTS_DIR +=     build/o/mac
}
unix {
  OBJECTS_DIR +=     build/o/unix
}
win32 {
  debug {
    OBJECTS_DIR +=       build/o/win32-debug
  }
  release {
    OBJECTS_DIR +=       build/o/win32-release
  }
}
DEFINES += 
