# $Id: lfhex.pro,v 1.1 2006/11/05 03:22:13 ganzhorn Exp $
TARGET  = lfhex
SOURCES = reader.cpp \
          save.cpp \
          translate.cpp \
          hexEditor.cpp \
          hexGui.cpp \
          grid.cpp \
          compareDlg.cpp \
          conversionDlg.cpp \
          cursor.cpp \
          delta.cpp \
          offsetConstraint.cpp \
          driver.cpp
HEADERS = offsetConstraint.hpp \
          hexGui.hpp \
          hexEditor.hpp \
          conversionDlg.hpp \
          compareDlg.hpp

# The following defines are used to select large file offsets.
# If your OS does not support this comment it out.
DEFINES += _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64
# If your machine is big endian then uncomment the following, otherwise the
# offset labels will be in little endian (which some find hard to read).
# DEFINES += WORDS_BIGENDIAN
lfhex.path  = /usr/local/bin
lfhex.files = lfhex
INSTALLS += lfhex
RESOURCES = images.qrc
YACCSOURCES += expr.y
LEXSOURCES += expr.l
QMAKE_LEX  = flex
QMAKE_YACC = bison
QMAKE_YACCFLAGS = -d -b expr -p expr

