// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "Modules/ModuleManager.h"

class FFoxUtilsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
