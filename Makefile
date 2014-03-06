include nall/Makefile
snes := snes
profile := compatibility
ui := ui-qt

# compiler
compiler:=llvm-g++
c       :=llvm-gcc -std=gnu99
cpp     := $(subst cc,++,$(compiler)) -std=gnu++0x
flags   := -O3 -fomit-frame-pointer -I. -I$(snes) -DQT_SHARED -I/usr/local/Cellar/qt/4.8.5/include -I/usr/local/Cellar/qt/4.8.5/include/QtGui -I/usr/local/Cellar/qt/4.8.5/include -I/usr/local/Cellar/qt/4.8.5/include/QtCore -I/usr/local/Cellar/qt/4.8.5/include/QtOpenGL -DDEBUGGER

link    := -F/usr/local/Cellar/qt/4.8.5/lib -framework QtOpenGL -F/usr/local/Cellar/qt/4.8.5/lib -framework QtGui -F/usr/local/Cellar/qt/4.8.5/lib -framework QtCore
objects :=

# profile-guided instrumentation
# flags += -fprofile-generate
# link += -lgcov

# profile-guided optimization
# flags += -fprofile-use

# platform
ifeq ($(platform),x)
  link += -s -ldl -lX11 -lXext
else ifeq ($(platform),osx)
else ifeq ($(platform),win)
  link += -mwindows
# link += -mconsole
  link += -mthreads -s -luuid -lkernel32 -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -lshell32 -lole32
  link += -enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc
else
  unknown_platform: help;
endif

# implicit rules
compile = \
  $(strip \
    $(if $(filter %.c,$<), \
      $(c) $(flags) $1 -c $< -o $@, \
      $(if $(filter %.cpp,$<), \
        $(cpp) $(flags) $1 -c $< -o $@ \
      ) \
    ) \
  )

%.o: $<; $(call compile)

all: build;

include $(snes)/Makefile
include $(ui)/Makefile

objects := $(patsubst %,obj/%.o,$(objects))

# targets
build: ui_build $(objects)
ifeq ($(platform),osx)
	test -d ../bsnes.app || mkdir -p ../bsnes.app/Contents/MacOS
	$(strip $(cpp) -o ../bsnes.app/Contents/MacOS/bsnes $(objects) $(link))
else
	$(strip $(cpp) -o out/bsnes $(objects) $(link))
endif

install:
ifeq ($(platform),x)
	install -D -m 755 out/bsnes $(DESTDIR)$(prefix)/bin/bsnes
	install -D -m 644 ui-qt/data/bsnes.png $(DESTDIR)$(prefix)/share/pixmaps/bsnes.png
	install -D -m 644 ui-qt/data/bsnes.desktop $(DESTDIR)$(prefix)/share/applications/bsnes.desktop
	gconftool-2 --type bool --set /desktop/gnome/interface/menus_have_icons true
endif

uninstall:
ifeq ($(platform),x)
	rm $(DESTDIR)$(prefix)/bin/bsnes
	rm $(DESTDIR)$(prefix)/share/pixmaps/bsnes.png
	rm $(DESTDIR)$(prefix)/share/applications/bsnes.desktop
endif

clean: ui_clean
	-@$(call delete,obj/*.o)
	-@$(call delete,obj/*.a)
	-@$(call delete,obj/*.so)
	-@$(call delete,obj/*.dylib)
	-@$(call delete,obj/*.dll)
	-@$(call delete,*.res)
	-@$(call delete,*.pgd)
	-@$(call delete,*.pgc)
	-@$(call delete,*.ilk)
	-@$(call delete,*.pdb)
	-@$(call delete,*.manifest)

archive-all:
	tar -cjf bsnes.tar.bz2 launcher libco nall obj out phoenix ruby snes ui-phoenix ui-qt Makefile cc.bat clean.bat sync.sh

help:;
