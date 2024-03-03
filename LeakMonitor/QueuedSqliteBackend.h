#pragma once

#include "QueuedBackend.h"

class QueuedSqliteBackend : public QueuedBackend
{
public:
	QueuedSqliteBackend();
   virtual ~QueuedSqliteBackend() override;

   virtual void initialize (DWORD pid) override;

protected:
   virtual void OnInitialized (DWORD pid) override;
   virtual void OnProcessEvent (const LEAKEVENT& event) override;

private:
   class Private;
   Private* mPrivate;
};
