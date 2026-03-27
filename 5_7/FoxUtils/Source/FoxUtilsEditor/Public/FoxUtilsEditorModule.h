// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "Modules/ModuleManager.h"

class FFoxUtilsEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Console command handlers */
	void HandleCreateShowcase(const TArray<FString>& Args);
	void HandleListNodes(const TArray<FString>& Args);
	void HandleSystemInfo(const TArray<FString>& Args);
	void HandleCountActors(const TArray<FString>& Args);
	void HandleTraceDebug(const TArray<FString>& Args);
	void HandleRunTests(const TArray<FString>& Args);
	void HandleCreateTestRunner(const TArray<FString>& Args);

	/** Registered console commands */
	TArray<IConsoleObject*> ConsoleCommands;
};
