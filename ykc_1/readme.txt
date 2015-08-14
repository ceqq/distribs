ENGLISH, RUSSIAN

vecm_hashx.h 1.0
High-performance transactional vector and hash with access by key and ordinal number.

Features

vecm
	- polymorphic container, using multipart storage
	- inserting/removing 1 element takes O(N^0.5), faster than std::vector in most cases
	- appending and removing at the end of sequence never relocates the existing elements
	- automatic space reserve and peak memory overhead on appending decrease
		asymptotically on container growing (25% at 1000 els., 1% at 1e6 etc.)
	- per-type configuration of data movability, accessibility and custom versions
		of T(), T(const T&), ~T(), internal exception handling, storage allocation etc.
hashx
	- strongly-typed container
	- very fast operation: ~2x in comp. to the widely known impls. in ins./rem., ~1x in find.
	- low peak time for any single ins./rem. operation (distributed reallocations and rehashing)
	- inherent hashlist behavior during the client making only inserts
	- index-based (vector-like) element access available
	- accepts keys of other type than its own, can compare on lookup without conversion
	- predefined default hash functions for most built-in types and standard strings as keys
Common
	- fully transactional containers, minimal possible time/space expenses on backup
	- storage space reserving does not relocate the existing elements
	- by default, space reserve is automatically shrinking on elements removal
	- non-throwing public functions, correct memory allocation errors handling
	- per-object integrity status
	- the client may freely relocate container objects in memory
	- code is in C++98 + long long ints, depends only on <new>; compilable in C++11
	- tested with variuos compilers in Windows, Linux, FreeBSD, OS X, Solaris
Possible flaws
	- no built-in synchronization and instance sharing (copy-on-write) of the containers
	- no system level exception handling
	- no STL-style interface
	- vecm: on trivial types, access by index, iterators, and appending may be slower than std::vector 
	- vecm: the empty container structures size is 48 + 8 * sizeof(T) bytes on arch. with 32-bit pointers

Package structure

vecm_hashx.h - standalone header, full implementation of the two containers

test/main.cpp - OS-independent console program performing full testing of vecm_hashx.h
test/txtsample.txt - readable text for string keys sampling

_add/experimental/ - experimental additions to vecm_hashx.h + code for testing them
_add/examples/ - example declarations for extending vecm_hashx.h functionality
_add/test_results/ - raw results of testing the containers on certain systems/compilers








vecm_hashx.h 1.0
���������� �������������� ������ � ��� � �������� �� ����� � ����������� ������.

������� ��������

vecm
	- ����������� ��������� � ������������ ���������� ���������
	- �������/�������� 1 �������� �� ����� O(N^0.5), ������� std::vector � ����������� �������
	- ����������/�������� ��������� � ����� ������������������ �� �������� �����������
		������������ ���������
	- �������������� ������ � ������� ��������� ������ ��� ���������� �������
		�������������� ��� ����� ���������� (25% �� 1000 ��., 1% �� 1e6 � �. �.)
	- �������������� ������������ �����: �������������� � ������, ����������� � ���������� ������
		T(), T(const T&), ~T(), ���������� ����� ��������� ����������, ��������� ������ � �. �.
hashx
	- �������������������� ���������
	- ������� �������� ��������: ~2x � ��������� � ���������������� ������������
		��� �������/��������, ~1x ��� ������
	- ������ ������� ����� ��������� �������/�������� (��������� ������ � �������� ������������
		�� ��������� ��������)
	- ���������� ������� ������� ��������� ����� ��������� ������� �� ��������
	- ������������ ������ � ��������� �� ������ (���������� �������)
	- ��������� ����� ����, ��������� �� ���� ����� � ����������, ��� �����������
	- ��������������� ���-������� ��� ����������� ���������� ����� � ����������� �����
����� �����������
	- ������ ���������������� �������� (�� ��� ������), ����������� ������� ������
		�� ��������� ����������� �������� ���, ��� ��� ���������
	- �� ���������, ��� �������� ���������, ������ ������ ��� �������� ����������� �������������
	- �������������� ����� �� �������� ����������� ������������ ���������
	- ������� ��������� ���������� �� ����������� ����������, �������� �������� ������
		�������������� ���������
	- ����� ����������� ������������� ��� ������� �������
	- ������ ����� �������� ���������� ���������� � ������
	- ��� ������ �� C++98 + long long int, ����������� ������ �� <new>; ��������� � C++11
	- �������������� � ���������� ������������� � Windows, Linux, FreeBSD, OS X, Solaris
��������� ����������
	- ���������� ���������� ������������� � ����������� ����������� ����� �����������
	- ���������� ��������� ���������� ���������� ������
	- ���������� ���������� � ����� STL
	- vecm: ��� ����������� �����, �������� ������� �� �������, ���������, � ���������� ��������
		� ����� ����� ���� ��������� std::vector
	- vecm: ������ �������� ������� ���������� 48 + 8 * sizeof(T) ���� �� ����������� � 32-������� �����������

��������� ������������

vecm_hashx.h - ��������� ������������ ����, ������ ���������� ���� �����������

test/main.cpp - ������������-����������� ���������� ���������
	��� ������� ������������ vecm_hashx.h
test/txtsample.txt - ����������� ����� ��� ��������� �������� ������ ���������� ����

_add/experimental/ - ����������������� ������� � ���������� vecm_hashx.h + ��� ��� �� ������������
_add/examples/ - ������� ���������� ��� ���������� ���������������� vecm_hashx.h
_add/test_results/ - �������� ������ ������������ ����������� �� ��������� ��������
