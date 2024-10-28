Name:           vdvge
Version:        1.2.0
Release:        1%{?dist}
Summary:        Volume Data Visualizer for Google Earth (VDVGE)

License:        GPL-3.0
URL:            https://github.com/kawaharas/VDVGE
Source0:        %{_sourcedir}/%{name}-%{version}-source.tar.gz

BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qttools-devel

%description
VDVGE is visualization software to generate a volume-rendering-like content for Google Earth.

%prep
%setup -q

%build
# qmake-qt6 VDVGE.pro
# %make_build
%{qt_make}
make

%install
# %make_install
# install -Dm644 %{_sourcedir}/README.md %{buildroot}%{_docdir}/%{name}/README.md
# install -Dm644 %{_sourcedir}/LICENSE %{buildroot}%{_docdir}/%{name}/LICENSE
rm -rf ${RPM_BUILD_ROOT}
mkdir -p ${RPM_BUILD_ROOT}/usr/bin
cp vdvge ${RPM_BUILD_ROOT}/usr/bin

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%{_bindir}/vdvge
# %{_docdir}/%{name}/README.md
%{_docdir}/%{name}/LICENSE

%changelog
* Fri Oct 25 2024 Shintaro Kawahara <kawahara@jamstec.go.jp> - 1.2.0
- Initial package for Qt6
* Wed Jun 11 2014 Tsuyoshi Yamaura <tyamaura@riken@jp> - 1.1.7-1
- Initial package
