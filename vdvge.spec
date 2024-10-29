Name:           vdvge
Version:        1.2.0
Release:        1%{?dist}
Summary:        Volume Data Visualizer for Google Earth (VDVGE)

License:        GPL-3.0
URL:            https://github.com/kawaharas/VDVGE
Source0:        %{_sourcedir}/%{name}-%{version}.tar.gz

# BuildRequires:  qt6-qtbase-devel
# BuildRequires:  qt6-qttools-devel
BuildRequires:  qt6-base-dev
BuildRequires:  qt6-tools-dev
BuildRequires:  libqt6core5compat6-dev
BuildRequires:  libqt6openglwidgets6-dev 
#BuildRequires:  qt6-qpa-plugins

%description
VDVGE is visualization software to generate a volume-rendering-like content for Google Earth.

%prep
%setup -q

%build
# qmake-qt6 VDVGE.pro
# %make_build
qmake
#make
%make_build

%install
rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}/usr/bin
cp build/vdvge ${RPM_BUILD_ROOT}/usr/bin

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%{_bindir}/vdvge
# %{_docdir}/%{name}/LICENSE

%changelog
* Fri Oct 25 2024 Shintaro Kawahara <kawahara@jamstec.go.jp> - 1.2.0
- Initial package for Qt6
* Wed Jun 11 2014 Tsuyoshi Yamaura <tyamaura@riken@jp> - 1.1.7-1
- Initial package
