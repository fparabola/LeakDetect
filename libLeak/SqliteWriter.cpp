#include "SqliteWriter.h"
#include "LeakFileStreamSerializer.h"

libLeak::SqliteWriter::SqliteWriter()
{
	extern HANDLE g_hEventSqliteWrite;
	extern HANDLE g_hEventSqliteWriteDone;
	extern const libLeak::LeakObject* g_sqliteWriteObject;
	extern const std::vector<libLeak::SYMBOL_ENTRY>* g_sqlWriteSymbolEntry;

	g_hEventSqliteWrite = CreateEvent(NULL, FALSE, FALSE, L"");
	g_hEventSqliteWriteDone = CreateEvent(NULL, FALSE, FALSE, L"");
}

/// Serializes a stacktrace
void libLeak::SqliteWriter::WriteStacktrace(uint32_t id, const std::vector<libLeak::SYMBOL_ENTRY>& symbols, uint64_t ts)
{
	extern HANDLE g_hEventSqliteWrite;
	extern HANDLE g_hEventSqliteWriteDone;
	extern const libLeak::LeakObject* g_sqliteWriteObject;
	extern const std::vector<libLeak::SYMBOL_ENTRY>* g_sqlWriteSymbolEntry;

	std::vector<uint8_t> bytes;
	LeakFileStreamSerializer::SerializeStacktrace(bytes, id, symbols, ts);
	g_sqliteWriteObject = (libLeak::LeakObject*)bytes.data();
	g_sqlWriteSymbolEntry = &symbols;
	SetEvent(g_hEventSqliteWrite);
	WaitForSingleObject(g_hEventSqliteWriteDone, INFINITE);

}

/// Serializes an allocation
void libLeak::SqliteWriter::WriteAllocation(uint32_t id, libLeak::PALLOCATION_EVENT allocation)
{
	extern HANDLE g_hEventSqliteWrite;
	extern HANDLE g_hEventSqliteWriteDone;
	extern const libLeak::LeakObject* g_sqliteWriteObject;
	extern const std::vector<libLeak::SYMBOL_ENTRY>* g_sqlWriteSymbolEntry;

	std::vector<uint8_t> bytes;
	LeakFileStreamSerializer::SerializeAllocation(bytes, allocation, id);
	g_sqliteWriteObject = (libLeak::LeakObject*)bytes.data();
	SetEvent(g_hEventSqliteWrite);
	WaitForSingleObject(g_hEventSqliteWriteDone, INFINITE);
}

/// Serializes a deallocation
void libLeak::SqliteWriter::WriteDeallocation(libLeak::PDELLOCATION_EVENT deallocation)
{
	extern HANDLE g_hEventSqliteWrite;
	extern HANDLE g_hEventSqliteWriteDone;
	extern const libLeak::LeakObject* g_sqliteWriteObject;
	extern const std::vector<libLeak::SYMBOL_ENTRY>* g_sqlWriteSymbolEntry;

	std::vector<uint8_t> bytes;
	LeakFileStreamSerializer::SerializeDeallocation(bytes, deallocation);
	g_sqliteWriteObject = (libLeak::LeakObject*)bytes.data();
	SetEvent(g_hEventSqliteWrite);
	WaitForSingleObject(g_hEventSqliteWriteDone, INFINITE);
}
