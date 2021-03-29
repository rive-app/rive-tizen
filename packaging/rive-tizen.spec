Name:       rive-tizen
Summary:    Rive Animation Tizen Runtime Engine
Version:    0.0.1
Release:    1
Group:      Graphics System/Rendering Engine
License:    MIT
URL:        https://github.com/rive-app/rive-tizen
Source0:    %{name}-%{version}.tar.gz

BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(thorvg)
BuildRequires:  pkgconfig(elementary)

BuildRequires:  meson
BuildRequires:  ninja
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Rive Animation Tizen Runtime Engine


%package devel
Summary:    Rive Animation Tizen Runtime Engine
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}


%description devel
Rive Animation Tizen Runtime Engine (devel)


%prep
%setup -q


%build

export DESTDIR=%{buildroot}

meson setup \
      --prefix /usr \
      --libdir %{_libdir} \
      builddir 2>&1
ninja \
      -C builddir \
      -j %(echo "`/usr/bin/getconf _NPROCESSORS_ONLN`")

%install

export DESTDIR=%{buildroot}

ninja -C builddir install

%files
%defattr(-,root,root,-)
%{_libdir}/librive_tizen.so.*
%manifest packaging/rive-tizen.manifest

%files devel
%defattr(-,root,root,-)
%{_includedir}/*.hpp
%{_libdir}/librive_tizen.so
%{_bindir}/rive_viewer
%{_libdir}/pkgconfig/rive_tizen.pc
/home/rive/resources/*.riv
