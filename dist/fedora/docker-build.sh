#!/bin/bash 

# Install dependencies
dnf -y install fedora-packager rpmdevtools wget \
        cmake docbook5-style-xsl xsltproc gcc-c++ hicolor-icon-theme librsvg2-tools \
	pkgconfig qt6-qtbase-devel qt6-qtconnectivity-devel qt6-qtpositioning-devel \
	qt6-qtserialport-devel qt6-qtbase-devel libusb1-devel yaml-cpp-devel qt6-qttools-devel

# Prepeare RPM build
rpmdev-setuptree

# Download sources
cd /root/rpmbuild/SOURCES 
wget https://github.com/hmatuschek/qdmr/archive/refs/tags/v0.13.0.tar.gz

# Copy specs and build RPM
cd /root/rpmbuld/SPECS
cp /mnt/qdmr.spec .
rpmbuild -ba qdmr.spec

