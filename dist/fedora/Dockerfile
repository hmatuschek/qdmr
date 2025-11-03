FROM fedora:rawhide

RUN dnf -y install fedora-packager rpmdevtools wget \
        cmake docbook5-style-xsl xsltproc gcc-c++ hicolor-icon-theme librsvg2-tools \
        pkgconfig qt6-qtbase-devel qt6-qtconnectivity-devel qt6-qtpositioning-devel \
        qt6-qtserialport-devel qt6-qtbase-devel libusb1-devel yaml-cpp-devel qt6-qttools-devel

RUN rpmdev-setuptree

CMD /bin/bash