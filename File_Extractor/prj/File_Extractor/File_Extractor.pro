QT       -= core gui

TARGET = File_Extractor
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../..

SOURCES += \
    ../../fex/Zlib_Inflater.cpp \
    ../../fex/Zip7_Extractor.cpp \
    ../../fex/Zip_Extractor.cpp \
    ../../fex/Rar_Extractor.cpp \
    ../../fex/Gzip_Reader.cpp \
    ../../fex/Gzip_Extractor.cpp \
    ../../fex/File_Extractor.cpp \
    ../../fex/fex.cpp \
    ../../fex/Data_Reader.cpp \
    ../../fex/blargg_errors.cpp \
    ../../fex/blargg_common.cpp \
    ../../fex/Binary_Extractor.cpp \
    ../../7z_C/LzmaLib.c \
    ../../7z_C/LzmaEnc.c \
    ../../7z_C/LzmaDec.c \
    ../../7z_C/Lzma86Enc.c \
    ../../7z_C/Lzma86Dec.c \
    ../../7z_C/Lzma2Enc.c \
    ../../7z_C/Lzma2Dec.c \
    ../../7z_C/LzFind.c \
    ../../7z_C/Delta.c \
    ../../7z_C/CpuArch.c \
    ../../7z_C/BraIA64.c \
    ../../7z_C/Bra86.c \
    ../../7z_C/Bra.c \
    ../../7z_C/Bcj2.c \
    ../../7z_C/Alloc.c \
    ../../7z_C/7zStream.c \
    ../../7z_C/7zIn.c \
    ../../7z_C/Ppmd7Enc.c \
    ../../7z_C/Ppmd7Dec.c \
    ../../7z_C/Ppmd7.c \
    ../../7z_C/7zDec.c \
    ../../7z_C/7zCrcOpt.c \
    ../../7z_C/7zCrc.c \
    ../../7z_C/7zBuf2.c \
    ../../7z_C/7zBuf.c \
    ../../7z_C/7zAlloc.c \
    ../../7z_C/MtCoder.c \
    ../../7z_C/LzFindMt.c \
    ../../7z_C/posix/Threads.c \
    ../../unrar/unrar.cpp \
    ../../unrar/unrar_open.cpp \
    ../../unrar/unrar_misc.cpp \
    ../../unrar/unpackinline.cpp \
    ../../unrar/unpack50frag.cpp \
    ../../unrar/unpack50.cpp \
    ../../unrar/unpack30.cpp \
    ../../unrar/unpack20.cpp \
    ../../unrar/unpack15.cpp \
    ../../unrar/unpack.cpp \
    ../../unrar/unicode.cpp \
    ../../unrar/timefn.cpp \
    ../../unrar/suballoc.cpp \
    ../../unrar/strfn.cpp \
    ../../unrar/rawread.cpp \
    ../../unrar/rarvmtbl.cpp \
    ../../unrar/rarvm.cpp \
    ../../unrar/pathfn.cpp \
    ../../unrar/model.cpp \
    ../../unrar/headers.cpp \
    ../../unrar/hash.cpp \
    ../../unrar/getbits.cpp \
    ../../unrar/extract.cpp \
    ../../unrar/encname.cpp \
    ../../unrar/crc.cpp \
    ../../unrar/coder.cpp \
    ../../unrar/blake2sp.cpp \
    ../../unrar/blake2s_sse.cpp \
    ../../unrar/blake2s.cpp \
    ../../unrar/arcread.cpp \
    ../../unrar/archive.cpp

HEADERS += \
    ../../fex/Zlib_Inflater.h \
    ../../fex/Zip7_Extractor.h \
    ../../fex/Zip_Extractor.h \
    ../../fex/Rar_Extractor.h \
    ../../fex/Gzip_Reader.h \
    ../../fex/Gzip_Extractor.h \
    ../../fex/File_Extractor.h \
    ../../fex/fex.h \
    ../../fex/Data_Reader.h \
    ../../fex/blargg_source.h \
    ../../fex/blargg_errors.h \
    ../../fex/blargg_endian.h \
    ../../fex/blargg_config.h \
    ../../fex/blargg_common.h \
    ../../fex/Binary_Extractor.h \
    ../../7z_C/Types.h \
    ../../7z_C/RotateDefs.h \
    ../../7z_C/LzmaTypes.h \
    ../../7z_C/LzmaLib.h \
    ../../7z_C/LzmaEnc.h \
    ../../7z_C/LzmaDec.h \
    ../../7z_C/Lzma86.h \
    ../../7z_C/Lzma2Enc.h \
    ../../7z_C/Lzma2Dec.h \
    ../../7z_C/LzFind.h \
    ../../7z_C/Delta.h \
    ../../7z_C/CpuArch.h \
    ../../7z_C/Bra.h \
    ../../7z_C/Bcj2.h \
    ../../7z_C/Alloc.h \
    ../../7z_C/7zIn.h \
    ../../7z_C/Ppmd7.h \
    ../../7z_C/Ppmd.h \
    ../../7z_C/7zCrc.h \
    ../../7z_C/7zBuf.h \
    ../../7z_C/7zAlloc.h \
    ../../7z_C/7z.h \
    ../../unrar/unrar.h \
    ../../unrar/unpack.hpp \
    ../../unrar/unicode.hpp \
    ../../unrar/timefn.hpp \
    ../../unrar/suballoc.hpp \
    ../../unrar/strfn.hpp \
    ../../unrar/rawread.hpp \
    ../../unrar/rarvm.hpp \
    ../../unrar/rar.hpp \
    ../../unrar/model.hpp \
    ../../unrar/headers5.hpp \
    ../../unrar/headers.hpp \
    ../../unrar/hash.hpp \
    ../../unrar/getbits.hpp \
    ../../unrar/encname.hpp \
    ../../unrar/compress.hpp \
    ../../unrar/coder.hpp \
    ../../unrar/blake2s.hpp \
    ../../unrar/array.hpp \
    ../../unrar/archive.hpp \
    ../../7z_C/MtCoder.h \
    ../../7z_C/LzFindMt.h \
    ../../7z_C/posix/Threads.h

OTHER_FILES += \
    ../../7z_C/readme.txt \
    ../../7z_C/lzma.txt \
    ../../7z_C/changes.txt \
    ../../7z_C/7zC.txt \
    ../../unrar/whatsnew.txt \
    ../../unrar/technote.txt \
    ../../unrar/readme.txt \
    ../../unrar/license.txt \
    ../../unrar/changes.txt


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
    DEFINES += _UNIX=1
}
