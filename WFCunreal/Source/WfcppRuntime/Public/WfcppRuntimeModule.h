#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FWfcppRuntimeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

DECLARE_LOG_CATEGORY_EXTERN(LogWFC, Log, All);