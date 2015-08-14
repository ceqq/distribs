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
Скоростные транзакционные вектор и хэш с доступом по ключу и порядковому номеру.

Краткое описание

vecm
	- полиморфный контейнер с многочастным хранилищем элементов
	- вставка/удаление 1 элемента за время O(N^0.5), быстрее std::vector в большинстве случаев
	- добавление/удаление элементов в конце последовательности не вызывает перемещений
		существующих элементов
	- автоматический резерв и пиковое выделение памяти при добавлении убывают
		асимптотически при росте контейнера (25% на 1000 эл., 1% на 1e6 и т. д.)
	- индивидуальная конфигурация типов: перемещаемость в памяти, доступность и клиентские версии
		T(), T(const T&), ~T(), внутренний режим обработки исключений, выделение памяти и т. д.
hashx
	- сильнотипизированный контейнер
	- высокая скорость операций: ~2x в сравнении с широкоизвестными реализациями
		при вставке/удалении, ~1x при поиске
	- низкое пиковое время одиночной вставки/удаления (выделение памяти и рехэшинг распределены
		по множеству операций)
	- сохранение порядка вставки элементов между запросами клиента на удаление
	- произвольный доступ к элементам по номеру (аналогично вектору)
	- принимает ключи типа, отличного от типа ключа в контейнере, без конвертации
	- предопределённые хэш-функции для большинства встроенных типов и стандартных строк
Общие особенности
	- полная транзакционность операций (всё или ничего), минимальные затраты памяти
		на резервное копирование объектов там, где это неизбежно
	- по умолчанию, при удалении элементов, резерв памяти под элементы сокращается автоматически
	- резервирование места не вызывает перемещений существующих элементов
	- функции открытого интерфейса не гененрируют исключений, ситуации нехватки памяти
		обрабатываются корректно
	- флаги целостности индивидуально для каждого объекта
	- клиент может свободно перемещать контейнеры в памяти
	- код модуля на C++98 + long long int, зависимость только от <new>; совместим с C++11
	- протестировано с различными компиляторами в Windows, Linux, FreeBSD, OS X, Solaris
Возможные недостатки
	- отсутствие встроенной синхронизации и отложенного копирования самих контейнеров
	- отсутствие обработки исключений системного уровня
	- отсутствие интерфейса в стиле STL
	- vecm: для тривиальных типов, операции доступа по индексу, итератору, и добавления элемента
		в конец могут быть медленнее std::vector
	- vecm: размер структур пустого контейнера 48 + 8 * sizeof(T) байт на архитектуре с 32-битными указателями

Структура дистрибутива

vecm_hashx.h - отдельный заголовочный файл, полная реализация двух контейнеров

test/main.cpp - платформенно-независимая консольная программа
	для полного тестирования vecm_hashx.h
test/txtsample.txt - читабельный текст для генерации тестовых ключей строкового типа

_add/experimental/ - экспериментальные добавки и приложения vecm_hashx.h + код для их тестирования
_add/examples/ - примеры объявлений для расширения функциональности vecm_hashx.h
_add/test_results/ - выходные данные тестирования контейнеров на некоторых системах
