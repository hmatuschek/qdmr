#!/bin/bash 

cp /mnt/qdmr.spec /root/rpmbuild/SPECS
VERSION="0.13.1"
TARBALL="v${VERSION}.tar.gz"

# Download sources
cd /root/rpmbuild/SOURCES 
if  [ ! -f $TARBALL ]; then
    wget https://github.com/hmatuschek/qdmr/archive/refs/tags/$TARBALL
fi

# Copy specs and build RPM
cd /root/rpmbuild/SPECS
rpmbuild -ba qdmr.spec


# Extract 
# cp /root/rpmbuild/RPMS/x86_64/*.rpm /mnt/