#pragma once

#include "libLeak.h"
#include "LeakObject.h"

namespace libLeak
{
   class SqliteWriter
   {
   public:
      /// Constructs a new SqliteWriter. The ownership of FILE* is 
      /// transferred to this instance.
      SqliteWriter ();

      /// Destructor. Closes the opened FILE*.
      virtual ~SqliteWriter() {};

      SqliteWriter (const SqliteWriter&) = delete;
      SqliteWriter (SqliteWriter&&) = delete;
      SqliteWriter& operator = (const SqliteWriter&) = delete;

      /// Serializes a stacktrace
      void WriteStacktrace (uint32_t id, const std::vector<libLeak::SYMBOL_ENTRY>& symbols, uint64_t ts);
      
      /// Serializes an allocation
      void WriteAllocation (uint32_t id, libLeak::PALLOCATION_EVENT allocation);

      /// Serializes a deallocation
      void WriteDeallocation (libLeak::PDELLOCATION_EVENT deallocation);
   };
}
