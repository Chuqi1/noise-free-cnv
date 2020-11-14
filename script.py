#!/usr/bin/python
import sys
import os
import posixpath
import subprocess
import ntpath
import glob
import math
import textwrap
import platform
import uuid
import shutil

#This script is used to build noise-free-cnv packages for different systems.
#Usage: "script.py source" or "script.py debian", "script.py windows" for a
#source package or a binary package. Supported operating systems currently
#are Windows and Debian.
#This script will generate an appropriate makefile, compile noise-free-cnv and
#package it.

name="noise-free-cnv"
headline="program for analyzing and manipulating DNA microarray data"

version_major="2"
version_minor="1"
revision="1"
system=platform.system()
architecture=platform.machine().replace("i486", "i386").replace("i686", "i386").replace("x86_64", "amd64")

email="<philip.development@googlemail.com>"
homepage="http://noise-free-cnv.sourceforge.net"

def preprocess(text):
	return (text.replace("$(NAME)", name)
		.replace("$(HEADLINE)", headline)
		.replace("$(VERSION)", version_major+"."+version_minor)
		.replace("$(VERSION_MAJOR)", version_major)
		.replace("$(VERSION_MINOR)", version_minor)
		.replace("$(REVISION)", revision)
		.replace("$(EMAIL)", email)
		.replace("$(HOMEPAGE)", homepage)
		.replace("$(ARCHITECTURE)", architecture)
		.replace("$(SYSTEM)", system))

gpl3_license=preprocess("""\
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.\
""")

description=preprocess("""\
The noise-free-cnv program package has been specifically developed to analyze
DNA microarray data for copy number variation and to manipulate such datasets
in order to reduce noise.

The central program of the noise-free-cnv package is noise-free-cnv-gtk, a
visual editor for interactive visualization and manipulation of DNA microarray
data. Besides functioning as a browser for direct inspection and verification
of alleged CNV findings, it allows the user to perform many operations on the
data to separate and eliminate noise components. 

As a second program, noiser-free-cnv-filter implements a specific algorithm for
system noise reduction of DNA microarray data. It is usable as a command line
program to be easily applied to a batch of datasets.

Both programs are able to read and write files suitable for PennCNV.\
""")

readme_text=preprocess("""\
$(NAME) is a $(HEADLINE).

"""+description+"""


homepage:	$(HOMEPAGE)
author:		Philip Ginsbach $(EMAIL)
""")

changelog_text=preprocess("""\
$(NAME) (1.0-1) optional; urgency=normal
  * initial version on the program
 -- Philip Ginsbach $(EMAIL)  Wed, 01 Feb 2012 00:00:00 +0000
""")

copyright_text=preprocess("""\
Authors: Philip Ginsbach $(EMAIL)

Copyright (c) 2010-2013 Philip Ginsbach

License:
    """+gpl3_license.replace("\n", "\n    ").replace("\n    \n", "\n\n")+"""

On Debian systems, the complete text of the GNU General Public
License, version 3, can be found in /usr/share/common-licenses/GPL-3.
""")

license_rtf_text=preprocess("""\
{\\rtf1
Copyright (c) 2010-2013 Philip Ginsbach $(EMAIL)
\\par \\par
"""+gpl3_license.replace("\n\n", " \\par \\par ").replace("\n", " ").replace(" \\par \\par ",  "\n\\par \\par\n")+"""
}
""")

desktop_text=preprocess("""\
[Desktop Entry]
Name=$(NAME)
Comment=$(HEADLINE)
Exec=$(NAME)-gtk
Terminal=false
Type=Application
Icon=$(NAME)
Categories=Education;Science;Biology;
GenericName=$(NAME)
""")

def generate_makefile_text():

	generated_text=""

	for path in (["share", "bin"]):
		for dirpath, dirnames, filenames in os.walk(path):
			dirpath=posixpath.normcase(dirpath)

			generated_text+="install -d "+posixpath.join("$(DESTDIR)", dirpath)+"\n"
			for filename in filenames:
				generated_text+="install -m 0644 "+posixpath.join(dirpath, filename)+" "+posixpath.join("$(DESTDIR)", dirpath, filename)+"\n"

	return textwrap.dedent(preprocess("""\
	CC=g++
	PKG_CONFIG=pkg-config
	SHARE_FOLDER=share/$(NAME)/

	CFLAGS=-g -c -Wall -O3 \\
		$(shell $(PKG_CONFIG) --cflags gtkmm-2.4 fftw3) \\
		-DNFCNV_VERSION_MAJOR=$(VERSION_MAJOR) \\
		-DNFCNV_VERSION_MINOR=$(VERSION_MINOR) \\
		-DNFCNV_SHARE_FOLDER='"$(SHARE_FOLDER)"'
	LDFLAGS1=-g -O3 -s \\
		$(shell $(PKG_CONFIG) --libs gtkmm-2.4 fftw3)
	LDFLAGS2=-g -O3 -s \\
		$(shell $(PKG_CONFIG) --libs glibmm-2.4 cairomm-1.0 fftw3)
	SOURCES1=$(shell ls src/[CGP]*.cc)
	SOURCES2=$(shell ls src/[CP]*.cc)
	OBJECTS1=$(SOURCES1:.cc=.o)
	OBJECTS2=$(SOURCES2:.cc=.o)

	build: $(SOURCES1) bin/$(NAME)-gtk bin/$(NAME)-filter

	bin/$(NAME)-gtk: $(OBJECTS1) src/$(NAME)-gtk.o
		$(CC) -o $@ $(OBJECTS1) src/$(NAME)-gtk.o $(LDFLAGS1)

	bin/$(NAME)-filter: $(OBJECTS2) src/$(NAME)-filter.o
		$(CC) -o $@ $(OBJECTS2) src/$(NAME)-filter.o $(LDFLAGS2)

	src/%.o: src/%.cc
		$(CC) -o $@ $< $(CFLAGS)

	install: bin/$(NAME)-gtk bin/$(NAME)-filter
		"""+generated_text.replace('\n', "\n\t\t")+"""
		install -m 0755 bin/* $(DESTDIR)/bin/

	clean:
		rm -f src/*.o
		rm -f bin/*

	.PHONY: build install clean
	"""))

def generate_wix_text():

	file_id_nr=0
	dir_id_nr=0
	cmp_id_nr=0

	generated_text_1=""

	current_path=""

	for dirpath, dirnames, filenames in os.walk("tmp"):
		dirpath=ntpath.normcase(dirpath)
		short_path=ntpath.relpath(dirpath, "tmp")
		relative_path=ntpath.relpath(short_path, current_path)

		if len(filenames)>0:
			for part in relative_path.split(ntpath.sep):
				if part=="..":
					generated_text_1+="</Directory>\n"
				elif part!=".":
					if short_path=="bin" and part=="bin": dir_id="BINDIR"
					elif short_path==ntpath.join("share", "doc", name) and part==name: dir_id="DOCDIR"
					else: dir_id="dir"+str(dir_id_nr); dir_id_nr+=1
					generated_text_1+="<Directory Id='"+dir_id+"' Name='"+part+"'>\n"
			current_path=short_path

			for filename in filenames:
				cmp_id="cmp"+str(cmp_id_nr); cmp_id_nr+=1
				generated_text_1+="  <Component Id='"+cmp_id+"' Guid='"+str(uuid.uuid4())+"' >\n"
				generated_text_1+="    <File Id='"+cmp_id+"' Name='"+filename+"' Source='"+ntpath.join(dirpath, filename)+"' KeyPath='yes' />\n"
				generated_text_1+="  </Component>\n"

	for part in current_path.split(ntpath.sep):
		generated_text_1+="</Directory>\n"


	generated_text_2=""

	while cmp_id_nr>0:
		generated_text_2+="<ComponentRef Id='cmp"+str(cmp_id_nr-1)+"' />\n"
		cmp_id_nr-=1

	ProgramFilesFolder="ProgramFilesFolder"
	if architecture=="x86": ProgramFilesFolder="ProgramFilesFolder"
	elif architecture=="AMD64": ProgramFilesFolder="ProgramFiles64Folder"

	return textwrap.dedent(preprocess("""\
		<?xml version='1.0' encoding='windows-1252'?>
		<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'
		  xmlns:util="http://schemas.microsoft.com/wix/UtilExtension">

		  <Product Name='$(NAME) $(VERSION)-1' Id='870ED3C0-8257-11E1-A82C-7BC54824019B' UpgradeCode='B76936B4-8257-11E1-8541-91C54824019B'
		    Language='1033' Codepage='1252' Version='$(VERSION).$(REVISION)' Manufacturer='Philip Ginsbach'>

		    <Package Id='*' Keywords='Installer' Description='$(NAME) $(VERSION)-1 Installer'
		      InstallerVersion='200' Compressed='yes' Languages='1033' SummaryCodepage='1252'
		      InstallScope='perMachine' />

		    <UIRef Id='WixUI_InstallDir' />
		    <WixVariable Id='WixUILicenseRtf' Value='tmp\\share\\doc\\$(NAME)\\license.rtf' />
		    <Property Id='WIXUI_INSTALLDIR' Value='APPLICATIONFOLDER' />

		    <Media Id='1' CompressionLevel='high' Cabinet='archive.cab' EmbedCab='yes' />

		    <Directory Id='TARGETDIR' Name='SourceDir'>
		      <Directory Id='"""+ProgramFilesFolder+"""' Name='PFiles'>
		        <Directory Id='APPLICATIONFOLDER' Name='$(NAME)'>
		          """+generated_text_1.replace('\n', "\n		          ")+"""
		        </Directory>
		      </Directory>

		      <Directory Id='ProgramMenuFolder' Name='Programs'>
		        <Directory Id='ProgramMenuDir' Name='$(NAME)'>
		          <Component Id='MenuShortcuts' Guid='"""+str(uuid.uuid4())+"""'>
		            <Shortcut Id='executable_shortcut'
		              Name='$(NAME)'
		              Description='$(HEADLINE)'
		              WorkingDirectory='BINDIR'
		              Target='[BINDIR]$(NAME)-gtk.exe' />
		            <Shortcut Id='readme_shortcut'
		              Name='readme'
		              Description='view $(NAME) readme'
		              WorkingDirectory='DOCDIR'
		              Target='[DOCDIR]readme.txt' />
		            <util:InternetShortcut Id='homepage_shortcut'
		              Name='project homepage'
		              Target='$(HOMEPAGE)' />
		            <Shortcut Id='uninstall_shortcut'
		              Name='uninstall $(NAME)'
		              Description='remove $(NAME) from this computer'
		              Target="[SystemFolder]msiexec.exe"
		              Arguments="/x [ProductCode]" />
		            <RemoveFolder Id='ProgramMenuDir' On='uninstall' />
		            <RegistryValue Root='HKCU' Key='Software\\$(NAME)' Name='installed' Type='integer' Value='1' KeyPath='yes'/>
		            <Environment Id="PATH" Name="Path" Value="[BINDIR]" Permanent="yes" Part="last" Action="set" System="yes" />
		          </Component>
		        </Directory>
		      </Directory>

		      <Directory Id='DesktopFolder' Name='Desktop'>
		        <Component Id='DesktopShortcuts' Guid='"""+str(uuid.uuid4())+"""'>
		          <Shortcut Id='executable_shortcut_desktop'
		            Name='$(NAME)'
		            Description='$(HEADLINE)'
		            Target='[BINDIR]$(NAME)-gtk.exe'
		            WorkingDirectory='BINDIR' />
		          <RegistryValue Root='HKCU' Key='Software\\$(NAME)' Name='installed' Type='integer' Value='1' KeyPath='yes'/>
		        </Component>
		      </Directory>
		    </Directory>

		    <Feature Id='main_program' Title='noise-free-cnv' Level='1' Display='expand'>
		      """+generated_text_2.replace('\n', "\n		      ")+"""
		      <Feature Id='start_menu_entries' Title='start menu entries' Level='1'>
		        <ComponentRef Id='MenuShortcuts' />
		      </Feature>
		      <Feature Id='desktop_shortcuts' Title='desktop shortcut' Level='1'>
		        <ComponentRef Id='DesktopShortcuts' />
		      </Feature>
		    </Feature>

		  </Product>
		</Wix>
		"""))

def install_runtime():
	hostpath="C:/Gtkmm/"

	if not posixpath.exists("tmp/bin"):
		os.makedirs("tmp/bin")

	shutil.copy2(hostpath+"bin/libfftw3-3.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgtkmm-2.4-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgdkmm-2.4-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libpangomm-1.4-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libatkmm-1.6-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libcairomm-1.0-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgiomm-2.4-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libglibmm-2.4-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libsigc-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgtk-win32-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgdk-win32-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgdk_pixbuf-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libpango-1.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libpangocairo-1.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libpangoft2-1.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libpangowin32-1.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libatk-1.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libcairo-2.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgio-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgmodule-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgobject-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgthread-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libglib-2.0-0.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libexpat-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libfontconfig-1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libpng14-14.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/zlib1.dll", "tmp/bin")
	shutil.copy2(hostpath+"redist/libgcc_s_sjlj-1.dll", "tmp/bin")

	if not posixpath.exists("tmp/etc/pango"):
		os.makedirs("tmp/etc/pango")
	if not posixpath.exists("tmp/etc/gtk-2.0"):
		os.makedirs("tmp/etc/gtk-2.0")
	if not posixpath.exists("tmp/lib/gtk-2.0/2.10.0/engines"):
		os.makedirs("tmp/lib/gtk-2.0/2.10.0/engines")
	if not posixpath.exists("tmp/share/themes/MS-Windows/gtk-2.0"):
		os.makedirs("tmp/share/themes/MS-Windows/gtk-2.0")

	shutil.copy2(hostpath+"etc/pango/pango.modules", "tmp/etc/pango")
	shutil.copy2(hostpath+"etc/gtk-2.0/gtk.immodules", "tmp/etc/gtk-2.0")
	shutil.copy2(hostpath+"etc/gtk-2.0/gtkrc", "tmp/etc/gtk-2.0")

	for file in os.listdir(hostpath+"lib/gtk-2.0/2.10.0/engines"):
		shutil.copy2(hostpath+"lib/gtk-2.0/2.10.0/engines/"+file, "tmp/lib/gtk-2.0/2.10.0/engines")
	shutil.copy2(hostpath+"share/themes/MS-Windows/gtk-2.0/gtkrc", "tmp/share/themes/MS-Windows/gtk-2.0")

	if architecture=="x86":
		shutil.copy2(hostpath+"redist/intl.dll", "tmp/bin")
		shutil.copy2(hostpath+"redist/freetype6.dll", "tmp/bin")
	elif architecture=="AMD64":
		shutil.copy2(hostpath+"redist/libintl-8.dll", "tmp/bin")
		shutil.copy2(hostpath+"redist/libfreetype-6.dll", "tmp/bin")

def generate_file(filename, text):
	file=open(filename, "wb")
	file.write(text.encode('utf8'))
	file.close()

def generate_win_file(filename, text):
	file=open(filename, "wb")
	file.write(text.replace("\n", "\r\n").encode('utf8'))
	file.close()

def clean_environment():
	subprocess.call(["make", "clean"])
	if os.path.exists("makefile"): os.remove("makefile")
	if os.path.exists("bin"): shutil.rmtree("bin")
	os.mkdir("bin")
	if os.path.exists("share/doc"): shutil.rmtree("share/doc")
	if os.path.exists("share/applications"): shutil.rmtree("share/applications")

def generate_environment():
	clean_environment()
	if not posixpath.exists(preprocess("share/doc/$(NAME)")):
		os.makedirs(preprocess("share/doc/$(NAME)"))
	if not posixpath.exists("share/applications"):
		os.makedirs("share/applications")
	generate_file(preprocess("share/doc/$(NAME)/copyright"), copyright_text)
	generate_file(preprocess("share/doc/$(NAME)/readme"), readme_text)
	generate_file(preprocess("share/doc/$(NAME)/changelog"), changelog_text)
	generate_file(preprocess("share/applications/$(NAME).desktop"), desktop_text)

	subprocess.call(["gzip", "-f", "-9", preprocess("share/doc/$(NAME)/changelog")])
	generate_file(preprocess("makefile"), generate_makefile_text());

def generate_win_environment():
	clean_environment()
	if not posixpath.exists(preprocess("share/doc/$(NAME)")):
		os.makedirs(preprocess("share/doc/$(NAME)"))
	generate_win_file(preprocess("share/doc/$(NAME)/license.rtf"), license_rtf_text)
	generate_win_file(preprocess("share/doc/$(NAME)/readme.txt"), readme_text)
	generate_win_file(preprocess("makefile"), generate_makefile_text());

def source_tarball():
	generate_environment()

	if posixpath.exists("tmp"): shutil.rmtree("tmp")
	os.makedirs(preprocess("tmp/$(NAME)-$(VERSION)"))

	shutil.copytree("bin", preprocess("tmp/$(NAME)-$(VERSION)/bin"))
	shutil.copytree("share", preprocess("tmp/$(NAME)-$(VERSION)/share"))
	shutil.copytree("src", preprocess("tmp/$(NAME)-$(VERSION)/src"))
	shutil.copy2("makefile", preprocess("tmp/$(NAME)-$(VERSION)"))
	shutil.copy2("script.py", preprocess("tmp/$(NAME)-$(VERSION)"))
	os.chdir("tmp")
	subprocess.call(["tar", "-c", "-z", "-f", preprocess("../$(NAME)-$(VERSION)-src.tar.gz"), preprocess("$(NAME)-$(VERSION)")])
	os.chdir("..")
	shutil.rmtree("tmp")

def debian_package():
	generate_environment()

	if posixpath.exists("tmp"): shutil.rmtree("tmp")
	os.makedirs("tmp/DEBIAN")
	os.makedirs("tmp/usr")
	os.chmod("tmp/usr", 0755)

	subprocess.call(["make", "install", "-B", "-j4",
			"CC=g++ -Wl,-z,relro",
			"PKG_CONFIG=pkg-config",
			"DESTDIR=tmp/usr",
			preprocess("SHARE_FOLDER=/usr/share/$(NAME)/")])
	shutil.move(preprocess("tmp/usr/share/doc/$(NAME)/changelog.gz"), preprocess("tmp/usr/share/doc/$(NAME)/changelog.Debian.gz"))

	installed_size = 0
	for dirpath, dirnames, filenames in os.walk("tmp/usr"):
		for filename in filenames:
			installed_size+=posixpath.getsize(posixpath.join(dirpath, filename))

	generate_file("tmp/DEBIAN/control", textwrap.dedent(preprocess("""\
		Package:        $(NAME)
		Version:        $(VERSION)-$(REVISION)
		Section:        science
		Priority:       optional
		Architecture:   $(ARCHITECTURE)
		Depends:        libc6, libfftw3-3, libgtkmm-2.4-1c2a
		Installed-Size: """+str(math.ceil(installed_size/1024))+"""
		Maintainer:     Philip Ginsbach $(EMAIL)
		Homepage:       $(HOMEPAGE)
		Description:    $(HEADLINE)
		"""))
		+" "+description.replace("\n\n", "\n.\n").replace("\n", "\n ")+"\n")

	subprocess.call(["fakeroot", "dpkg-deb", "-b", "tmp", preprocess("$(NAME)_$(VERSION)-$(REVISION)_$(ARCHITECTURE).deb")])
	shutil.rmtree("tmp")

def windows_package():
	generate_win_environment()

	if not posixpath.exists("tmp"): os.mkdir("tmp")

	if architecture=="x86":
		subprocess.call(["make", "install", "-B", "-j4",
			"CC=g++ -static-libgcc -static-libstdc++ -mwindows",
			"PKG_CONFIG=pkg-config",
			"DESTDIR=tmp/",
			preprocess("SHARE_FOLDER=./../share/$(NAME)/")])

	elif architecture=="AMD64":
		subprocess.call(["make", "install", "-B", "-j4",
			"CC=g++ -static-libgcc -static-libstdc++ -mwindows",
			"PKG_CONFIG=pkg-config",
			"DESTDIR=tmp/",
			preprocess("SHARE_FOLDER=./../share/$(NAME)/")])

	install_runtime()

	generate_win_file("setup.wxs", generate_wix_text())

	if architecture=="x86":
		subprocess.call(["candle", "setup.wxs", "-ext", "WiXUtilExtension", "-arch", "x86"])
		subprocess.call(["light", "setup.wixobj", "-ext", "WiXUtilExtension",
			"-ext", "WiXUiExtension", "-o", preprocess("$(NAME)-$(VERSION)-$(REVISION)-win32.msi")])
		os.remove(preprocess("$(NAME)-$(VERSION)-$(REVISION)-win32.wixpdb"))

	elif architecture=="AMD64":
		subprocess.call(["candle", "setup.wxs", "-ext", "WiXUtilExtension", "-arch", "x64"])
		subprocess.call(["light", "setup.wixobj", "-ext", "WiXUtilExtension",
			"-ext", "WiXUiExtension", "-o", preprocess("$(NAME)-$(VERSION)-$(REVISION)-win64.msi")])
		os.remove(preprocess("$(NAME)-$(VERSION)-$(REVISION)-win64.wixpdb"))

	os.remove("setup.wixobj")
	os.remove("setup.wxs")

	generate_win_file(preprocess("tmp/$(NAME).bat"),
		preprocess("cd bin\nstart $(NAME)-gtk.exe\n"))

	os.chdir("tmp")

	if architecture=="x86":
		subprocess.call(["zip", preprocess("../$(NAME)-$(VERSION)-$(REVISION)-win32.zip"), "-9", "-r", "*"])
	elif architecture=="AMD64":
		subprocess.call(["zip", preprocess("../$(NAME)-$(VERSION)-$(REVISION)-win64.zip"), "-9", "-r", "*"])

	os.chdir("..")
	shutil.rmtree("tmp")


if len(sys.argv) >= 2 and sys.argv[1]=="source":
	source_tarball()
	clean_environment()

elif len(sys.argv) >= 2 and sys.argv[1]=="debian":
	debian_package()

elif len(sys.argv) >= 2 and sys.argv[1]=="windows":
	windows_package()

else: generate_environment()
