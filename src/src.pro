TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
  bench \
  previewGenerator \
  runtime \

include(src.pri)
