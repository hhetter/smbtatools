#
# spec file for package smbtatools
#
# Copyright (c) 2010 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild


Name:           smbtatools
BuildRequires:  cmake libtalloc-devel ncurses-devel libsmbclient-devel
BuildRequires:  sqlite3-devel libiniparser-devel
Requires:	sqlite3 ncurses
License:        GPLv3+
Group:          Productivity/Networking/Samba
Version:        1.2.1
Release:        1
Summary:        Tools for configuration and query of SMB Traffic Analyzer
Url:            http://github.com/hhetter/smbtatools
Source0:        %{name}-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
The smbtatools contain utility programs helping the end user to query the database that is created by the smbtad program. smbtatools is part of the SMB Traffic Analyzer project, which allows to create statistics about the data flow on a Samba network. For more information, please see: http://holger123.wordpress.com/smb-traffic-analyzer/

%prep
%setup -q

%build
if test ! -e "build"; then
  %{__mkdir} build
fi

pushd build
cmake \
  -DCMAKE_C_FLAGS:STRING="%{optflags}" \
  -DCMAKE_CXX_FLAGS:STRING="%{optflags}" \
  -DCMAKE_SKIP_RPATH=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX=%{_prefix} \
  -DSYSCONF_INSTALL_DIR=%{_sysconfdir} \
%if %{_lib} == lib64
  -DLIB_SUFFIX=64 \
%endif
  %{_builddir}/%{name}-%{version}
%__make %{?jobs:-j%jobs} VERBOSE=1
popd build

%install
pushd build
%if 0%{?suse_version}
%makeinstall
%else
make DESTDIR=%{buildroot} install
%endif
popd build


%clean
%__rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_bindir}/smbtaquery
%{_bindir}/smbtamonitor
%{_bindir}/smbtatorture

%{_mandir}/man?/smbtaquery.*
%{_mandir}/man?/smbtamonitor.*
%{_mandir}/man?/smbtatorture.*

%doc doc/smbta-guide.html

%changelog
