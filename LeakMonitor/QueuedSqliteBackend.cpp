#include "QueuedSqliteBackend.h"

#include <iostream>
#include <filesystem>
#include <unordered_set>
#include "../LeakConverter/GenerateSQLite.h"
#include "SqliteWriter.h"

/// Assuming this is declared somewhere.
extern void LogMessage (const std::string& message);

class QueuedSqliteBackend::Private
{
   friend class ::QueuedSqliteBackend;
   
   std::unordered_set<uint32_t> known_stacktraces;
   std::shared_ptr<libLeak::SqliteWriter> writer;

   Private ()
   {
   }

   ~Private ()
   {
   }

   void initialize (DWORD pid)
   {
       GenerateSQLite(pid);
   }

   void WriteEvent (const LEAKEVENT& event)
   {
      if (event.allocation != NULL)
      {
         // Write unique stacktraces once..
         uint32_t stacktrace_id = libLeak::CreateUniqueId (event.symbols);
         if (known_stacktraces.find (stacktrace_id) == known_stacktraces.end ())
         {
            writer->WriteStacktrace (stacktrace_id, event.symbols, event.allocation->TimestampEpochSeconds);
            known_stacktraces.insert (stacktrace_id);
         }

         // Serialize the allocation..
         writer->WriteAllocation (stacktrace_id, event.allocation);
      }
      else if (event.deallocation != NULL)
      {
         // Serialize the deallocation..
         writer->WriteDeallocation (event.deallocation);
      }
   }

   // Get current date/time, format is YYYY-MM-DD.HH:mm
   static const std::string time_str() 
   {
      time_t now = time(0);
      struct tm tstruct;
      char buf[80];
      localtime_s(&tstruct, &now);
      strftime(buf, sizeof(buf), "%Y-%m-%d.%H-%M", &tstruct);
      return buf;
   }

   static std::filesystem::path GetSessionDirectory (DWORD pid)
   {
      wchar_t path[MAX_PATH];
      memset (path, 0, sizeof (path));
      GetModuleFileName (NULL, path, MAX_PATH);

      std::wstring m (path);
      m = m.erase (m.find_last_of (L"\\") + 1);

      return std::filesystem::path (m) / "Logs" / (std::to_string (pid) + " - " + time_str ());
   }
};

QueuedSqliteBackend::QueuedSqliteBackend ()
   : mPrivate(new Private())
{
}

QueuedSqliteBackend::~QueuedSqliteBackend ()
{
   delete mPrivate;
   mPrivate = nullptr;
}

void QueuedSqliteBackend::initialize (DWORD pid)
{
   // Initialize session writer..
   mPrivate->initialize (pid);

   // Initialize base class..
   __super::initialize (pid);
}

void QueuedSqliteBackend::OnInitialized (DWORD pid)
{
   LogMessage ("Initialized QueuedSqliteBackend for PID " + std::to_string (pid));
}

void QueuedSqliteBackend::OnProcessEvent (const LEAKEVENT& event)
{
   mPrivate->WriteEvent (event);
}
