#include "WfcppRuntimeModule.h"

IMPLEMENT_GAME_MODULE(FWfcppRuntimeModule, WfcppRuntime);

#define LOCTEXT_NAMESPACE "FWfcppRuntimeModule"

void FWfcppRuntimeModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("StartupModule() WFC-runtime"));
}
void FWfcppRuntimeModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("ShutdownModule() WFC-runtime"));
}

DEFINE_LOG_CATEGORY(LogWFC);

#undef LOCTEXT_NAMESPACE
