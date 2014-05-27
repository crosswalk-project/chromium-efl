Name: org.tizen.chromium-efl
Summary: Chromium EFL
# Set by by scripts/update-chromium-version.sh
%define ChromiumVersion 34.1847.76
%define Week 19
Version: %{ChromiumVersion}.%{Week}
Release: 1
Group: Applications/Internet
License: LGPLv2.1 or BSD

Source0: %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(post): xkeyboard-config
Requires(postun): /sbin/ldconfig
BuildRequires: which, vi, python, python-xml, bison, flex, gperf, gettext, perl, edje-bin
BuildRequires: libjpeg-turbo-devel, expat-devel, libasound-devel, libhaptic, libcap-devel
BuildRequires: pkgconfig(glesv2)
BuildRequires: pkgconfig(libpulse)
BuildRequires: pkgconfig(recordproto)
BuildRequires: pkgconfig(nss)
BuildRequires: pkgconfig(gconf-2.0)
BuildRequires: pkgconfig(libpci)
BuildRequires: pkgconfig(pangocairo)
BuildRequires: pkgconfig(libudev)
BuildRequires: pkgconfig(fontconfig)
BuildRequires: pkgconfig(freetype2)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(harfbuzz)
BuildRequires: pkgconfig(icu-i18n)
BuildRequires: pkgconfig(openssl)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(libxslt)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(capi-system-sensor)
BuildRequires: pkgconfig(capi-system-info)
BuildRequires: pkgconfig(capi-location-manager)
BuildRequires: pkgconfig(location)
BuildRequires: pkgconfig(gles20)
BuildRequires: pkgconfig(libpng12)
BuildRequires: pkgconfig(libusb-1.0)
BuildRequires: pkgconfig(speex)
BuildRequires: pkgconfig(flac)
BuildRequires: pkgconfig(minizip)
BuildRequires: pkgconfig(xrandr)
BuildRequires: pkgconfig(xcomposite)
BuildRequires: pkgconfig(xext)
BuildRequires: pkgconfig(xi)
BuildRequires: pkgconfig(xt)
BuildRequires: pkgconfig(xfixes)
BuildRequires: pkgconfig(xtst)
BuildRequires: pkgconfig(xdamage)
BuildRequires: pkgconfig(xcursor)
BuildRequires: pkgconfig(libdrm)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore-x)
BuildRequires: pkgconfig(ecore-evas)
BuildRequires: pkgconfig(ecore-input)
BuildRequires: pkgconfig(ecore-imf-evas)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(libexif)
BuildRequires: pkgconfig(nspr)
BuildRequires: pkgconfig(zlib)
%if %{!?TIZEN_PROFILE_TV:1}%{?TIZEN_PROFILE_TV:0}
BuildRequires: pkgconfig(ewebkit2-ext)
BuildRequires: bzip2-devel
BuildRequires: pkgconfig(vpx)
BuildRequires: pkgconfig(efl-assist)
%endif
BuildRequires: pkgconfig(gstreamer-0.10)
BuildRequires: pkgconfig(gstreamer-plugins-base-0.10)
BuildRequires: pkgconfig(gstreamer-app-0.10)
BuildRequires: pkgconfig(capi-media-camera)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(libdri2)
BuildRequires: pkgconfig(libtbm)
BuildRequires: pkgconfig(x11)
BuildRequires: pkgconfig(feedback)

%description
Browser Engine based on Chromium EFL (Shared Library)

%package devel
Summary: Chromium EFL
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
%description devel
Browser Engine dev library based on Chromium EFL (developement files)

# Directory for internal chromium executable components
%global CHROMIUM_EXE_DIR %{_libdir}/%{name}
# Constant read only data used by chromium-efl port
%global CHROMIUM_DATA_DIR %{_datadir}/%{name}
# Web Databse read write data used by chromium-efl port
%global CHROMIUM_WEBDB_DIR /opt/usr/apps/%{name}

%prep
%setup -q

%build

# workaround for new nss library : search it in /usr/lib first, rather than /lib (system nss)
export LD_RUN_PATH=%{_libdir}
#/usr/lib
export BUILDING_WITH_GBS=1

# Temporary workaround
%ifarch %{arm}
export CFLAGS="$(echo $CFLAGS | sed 's/-mfpu=[a-zA-Z0-9-]*/-mfpu=neon/g')"
export CXXFLAGS="$(echo $CXXFLAGS | sed 's/-mfpu=[a-zA-Z0-9-]*/-mfpu=neon/g')"
export FFLAGS="$(echo $FFLAGS | sed 's/-mfpu=[a-zA-Z0-9-]*/-mfpu=neon/g')"
%else
export CFLAGS="$(echo $CFLAGS | sed 's/-Wl,--as-needed//g')"
export CXXFLAGS="$(echo $CXXFLAGS | sed 's/-Wl,--as-needed//g')"
%endif

%if 0%{?nodebug}
CFLAGS=$(echo $CFLAGS | sed 's/ -g / /')
CXXFLAGS=$(echo $CXXFLAGS | sed 's/ -g / /')
%endif

%if %{!?TIZEN_PROFILE_TV:1}%{?TIZEN_PROFILE_TV:0}
%define OUTPUT_BUILD_PROFILE_TARGET mobile
%else
%define OUTPUT_BUILD_PROFILE_TARGET tv
%endif

%ifarch %{arm}
%define EFL_TARGET arm
%else
%if 0%{?simulator}
%define EFL_TARGET emulator
%else
%define EFL_TARGET i386
%endif
%endif

#TODO: This hardcoding should go
%define INSTALL_ROOT /home/abuild/rpmbuild/BUILDROOT/%{name}-%{version}-%{release}.arm

. ./build/envsetup.sh

%define OUTPUT_BASE_FOLDER out.%{OUTPUT_BUILD_PROFILE_TARGET}.%{EFL_TARGET}

#set build mode
%if 0%{?_debug_mode}
%global OUTPUT_FOLDER %{OUTPUT_BASE_FOLDER}/Debug
# Building the RPM in the GBS chroot fails with errors such as
#   /usr/lib/gcc/i586-tizen-linux/4.7/../../../../i586-tizen-linux/bin/ld:
#       failed to set dynamic section sizes: Memory exhausted
# For now, work around it by passing a GNU ld-specific flag that optimizes the
# linker for memory usage.
export LDFLAGS="${LDFLAGS} -Wl,--no-keep-memory"
%else
%global OUTPUT_FOLDER %{OUTPUT_BASE_FOLDER}/Release
%endif
# to workaround mess in ./build/envsetup.sh
export GYP_GENERATOR_OUTPUT=%{OUTPUT_BASE_FOLDER}

if type ccache &> /dev/null; then
export CCACHE_CPP2=yes
export CCACHE_SLOPPINESS=time_macros
export CCACHE_BASEDIR=$PWD/src
export CCACHE_DIR=$PWD/%{OUTPUT_BASE_FOLDER}.ccache
export CCACHE_COMPILERCHECK=content
CCACHESIZE=$(ccache -s | grep max | grep -oE "[0-9]+" | head -1)
  if [ $CCACHESIZE == "1" ]; then
    ccache -M 10
  fi
fi

#gyp generate
%if %{?_skip_gyp:0}%{!?_skip_gyp:1}
gyp_chromiumefl \
  -Dexe_dir="%{CHROMIUM_EXE_DIR}" \
  -Ddata_dir="%{CHROMIUM_DATA_DIR}" \
  -Dedje_dir="%{CHROMIUM_DATA_DIR}"/themes \
  -Dwebdb_dir="%{CHROMIUM_WEBDB_DIR}"/data/db
%endif

ninja %{_smp_mflags} -C"%{OUTPUT_FOLDER}"

%install
install -d "%{INSTALL_ROOT}"%{_sysconfdir}/smack/accesses2.d
install -d "%{INSTALL_ROOT}"%{_libdir}/pkgconfig
install -d "%{INSTALL_ROOT}"%{_includedir}/v8
install -d "%{INSTALL_ROOT}%{CHROMIUM_EXE_DIR}"
install -d "%{INSTALL_ROOT}%{CHROMIUM_DATA_DIR}"/themes

# Generate pkg-confg file
sed -e 's#?VERSION?#%{version}#' pkgconfig/chromium-efl.pc.in > "%{OUTPUT_FOLDER}"/chromium-efl.pc

install -m 0755 "%{OUTPUT_FOLDER}"/lib/libchromium-efl.so    "%{INSTALL_ROOT}"%{_libdir}

install -m 0755 "%{OUTPUT_FOLDER}"/libffmpegsumo.so  "%{INSTALL_ROOT}%{CHROMIUM_EXE_DIR}"
install -m 0755 "%{OUTPUT_FOLDER}"/efl_webprocess    "%{INSTALL_ROOT}%{CHROMIUM_EXE_DIR}"
install -m 0755 "%{OUTPUT_FOLDER}"/content_shell.pak "%{INSTALL_ROOT}%{CHROMIUM_EXE_DIR}"
install -m 0644 "%{OUTPUT_FOLDER}"/resources/*.edj   "%{INSTALL_ROOT}%{CHROMIUM_DATA_DIR}"/themes

install -m 0644 "%{OUTPUT_FOLDER}"/*.pc              "%{INSTALL_ROOT}"%{_libdir}/pkgconfig/
install -m 0644 src/v8/include/*.h                   "%{INSTALL_ROOT}"%{_includedir}/v8/

%post
# apply smack rule
smack_reload.sh

# create webdatabase directory
mkdir -p %{CHROMIUM_WEBDB_DIR}/data/db/

# create dummy webdatabase
if [ ! -f %{CHROMIUM_WEBDB_DIR}/data/db/FormData.db ];
then
    sqlite3 %{CHROMIUM_WEBDB_DIR}/data/db/FormData.db 'PRAGMA journal_mode=PERSIST;
    create table dummy_table(id integer primary key);'
fi
if [ ! -f %{CHROMIUM_WEBDB_DIR}/data/db/LoginData.db ];
then
    sqlite3 %{CHROMIUM_WEBDB_DIR}/data/db/LoginData.db 'PRAGMA journal_mode=PERSIST;
    create table dummy_table(id integer primary key);'
fi

# change db file owner & permission
chmod 660 %{CHROMIUM_WEBDB_DIR}/data/db/FormData.db
chmod 660 %{CHROMIUM_WEBDB_DIR}/data/db/FormData.db-journal
chmod 660 %{CHROMIUM_WEBDB_DIR}/data/db/LoginData.db
chmod 660 %{CHROMIUM_WEBDB_DIR}/data/db/LoginData.db-journal

chown -R 5000:5000 %{CHROMIUM_WEBDB_DIR}/

# Apply SMACK label to database files
if [ -f /usr/lib/rpm-plugins/msm.so ]
then
    chsmack -a 'chromium-efl' %{CHROMIUM_WEBDB_DIR}/data/db/*.db*
fi

%postun

%files
%manifest packaging/%{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libchromium-efl.so
%{CHROMIUM_EXE_DIR}/efl_webprocess
%{CHROMIUM_EXE_DIR}/libffmpegsumo.so
%{CHROMIUM_EXE_DIR}/content_shell.pak
%{CHROMIUM_DATA_DIR}/themes/*.edj

%files devel
%defattr(-,root,root,-)
%{_includedir}/v8/*
%{_libdir}/pkgconfig/*.pc
