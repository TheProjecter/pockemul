CONFIG += build_all \
    debug_and_release \
    qt \
    resources \
    thread \
    warn_on
DEFINES +=
FORMS += ui/about.ui \
    ui/dialoganalog.ui \
    ui/dialogconnectorlink.ui \
    ui/dialogdump.ui \
    ui/dialogkeylist.ui \
    ui/dialoglog.ui \
    ui/pockemul.ui \
    ui/serialconsole.ui \
    ui/startup.ui \
    ui/dialogpotar.ui \
    ui/dialogide.ui
HEADERS += src/core/Connect.h \
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
    src/core/autoupdater.h \
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
    src/core/global.h \
    src/core/hexviewer.h \
    src/core/init.h \
    src/core/keybce150.h \
    src/core/keybce152.h \
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
    src/core/lfhex/save.h \
    src/core/lfhex/translate.h \
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
    src/cpu/sc62015.h \
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
    src/cpu/z80.h \
    src/cpu/z80memory.h \
    src/cpu/lh5803.h \
    src/machine/pc1600.h \
    src/machine/ce1600p.h \
    src/cpu/hd61102.h \
    src/core/Lcdc_pc1600.h \
    src/cpu/z80-2.h \
    src/cpu/lu57813p.h \
    src/core/Keyb1600.h \
    src/machine/potar.h \
    src/core/dialogpotar.h \
    src/machine/cmotor.h \
    src/cpu/tc8576p.h \
    src/core/cextension.h \
    src/machine/pc2500.h \
    src/core/Keyb2500.h \
    src/machine/ce515p.h \
    src/machine/pc1421.h \
    src/machine/pc1280.h \
    src/core/Keyb1280.h \
    src/core/qhexedit.h \
    src/core/qhexedit_p.h \
    src/machine/ce140p.h \
    src/core/weblinksparser.h \
    src/core/wavfile.h \
    src/core/lcc/lcc.h \
    src/core/dialogide.h \
    src/core/ide/highlighter.h \
    src/core/lcc/lcpp.h \
    src/core/lcc/codegen.h \
    src/core/lcc/pasm.h \
    resources/lcc/model/PC-1350/stdio.h \
    resources/lcc/model/PC-1350/stdio.h \
    src/core/lcc/cstdlib.h \
    resources/lcc/model/PC-1350/graph.h \
    src/core/lcc/parser/parser.h
INCLUDEPATH += . \
    src/core \
    src/cpu \
    src/machine
mac:INCLUDEPATH += /Users/svp/Library/fmodapi375mac/api/inc
win32:INCLUDEPATH += ../fmodapi375win/api/inc
LANGUAGE += C++
win32:LIBS += ../fmodapi375win/api/lib/libfmod.a
mac:LIBS += /Users/svp/Library/fmodapi375mac/api/lib/libfmodx86.a
LICENSE += GPL
MOC_DIR += build/moc
OPENEDFILES += 
QT += core \
    gui \
    network \
    opengl \
    xml \
    multimedia
RCC_DIR += build/rcc
RESOURCES += resources/ext.qrc \
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
    src/core/lfhex/lfhex.qrc \
    resources/pc1600.qrc \
    resources/pc2500.qrc \
    resources/pc1421.qrc \
    resources/pc1425.qrc \
    resources/pc1460.qrc \
    resources/pc1280.qrc \
    resources/keymap.qrc \
    resources/stdlibs.qrc
SOURCES += src/core/Connect.cpp \
    src/core/Dasm.cpp \
    src/core/Debug.cpp \
    src/core/Inter.cpp \
    src/core/Keyb.cpp \
    src/core/Lcdc.cpp \
    src/core/Log.cpp \
    src/core/analog.cpp \
    src/core/autoupdater.cpp \
    src/core/clink.cpp \
    src/core/cprinter.cpp \
    src/core/dialogabout.cpp \
    src/core/dialoganalog.cpp \
    src/core/dialogconsole.cpp \
    src/core/dialogdump.cpp \
    src/core/dialogkeylist.cpp \
    src/core/dialoglog.cpp \
    src/core/dialogstartup.cpp \
    src/core/hexviewer.cpp \
    src/core/init.cpp \
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
    src/core/lfhex/translate.cpp \
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
    src/cpu/sc62015.cpp \
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
    src/cpu/z80.cpp \
    src/cpu/lh5803.cpp \
    src/machine/pc1600.cpp \
    src/machine/ce1600p.cpp \
    src/cpu/z80_dasm.cpp \
    src/cpu/hd61102.cpp \
    src/core/Lcdc_pc1600.cpp \
    src/cpu/lu57813p.cpp \
    src/machine/potar.cpp \
    src/core/dialogpotar.cpp \
    src/machine/cmotor.cpp \
    src/cpu/tc8576p.cpp \
    src/core/cextension.cpp \
    src/machine/pc2500.cpp \
    src/machine/ce515p.cpp \
    src/machine/pc1421.cpp \
    src/machine/pc1280.cpp \
    src/core/qhexedit.cpp \
    src/core/qhexedit_p.cpp \
    src/machine/ce140p.cpp \
    src/core/weblinksparser.cpp \
    src/core/wavfile.cpp \
    src/core/lcc/lcc.cpp \
    src/core/lcc/calcunit.cpp \
    src/core/dialogide.cpp \
    src/core/ide/highlighter.cpp \
    src/core/lcc/lcpp.cpp \
    src/core/lcc/codegen.cpp \
    src/core/lcc/pasm.cpp \
    src/core/lcc/cstdlib.cpp \
    src/core/lcc/parser/parser.cpp
TARGET = Pockemul
TEMPLATE = app
TRANSLATIONS += pockemul_fr.ts
UI_DIR += build/ui
mac:OBJECTS_DIR += build/o/mac
unix:OBJECTS_DIR += build/o/unix
win32 { 
    debug:OBJECTS_DIR += build/o/win32-debug
    release:OBJECTS_DIR += build/o/win32-release
}

OTHER_FILES += \
    resources/keymap/trspc2.map \
    resources/keymap/pc2500.map \
    resources/keymap/pc1600.map \
    resources/keymap/pc1500.map \
    resources/keymap/pc1450.map \
    resources/keymap/pc1403.map \
    resources/keymap/pc1401.map \
    resources/keymap/pc1360.map \
    resources/keymap/pc1350.map \
    resources/keymap/pc1280.map \
    resources/keymap/pc1250.map \
    resources/keymap/ce1600p.map \
    resources/keymap/ce150.map \
    resources/pockemul/release_notes.html \
    resources/keymap/ce125tape.map \
    resources/keymap/ce126.map \
    resources/keymap/ce125.map \
    resources/keymap/ce129.map \
    resources/keymap/ce123.map \
    resources/keymap/ce120.map \
    resources/keymap/263591.map \
    resources/keymap/ce140p.map \
    resources/weblinks.xml
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
