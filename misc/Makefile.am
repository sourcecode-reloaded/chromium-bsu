MAINTAINERCLEANFILES = Makefile.in

dist_man_MANS = chromium-bsu.6

icon_DATA = $(top_srcdir)/data/png/hero.png
icondir = $(datarootdir)/icons/hicolor/64x64/apps

install-data-hook:
	mv $(icondir)/hero.png $(icondir)/chromium-bsu.png

menu_in_files = chromium-bsu.desktop.in
menu_DATA = $(menu_in_files:.desktop.in=.desktop)
menudir = $(datarootdir)/applications

CLEANFILES = $(menu_DATA)
EXTRA_DIST = $(menu_in_files)

%.desktop: %.desktop.in
	msgfmt --desktop -d $(top_srcdir)/po --template $< -o $@
