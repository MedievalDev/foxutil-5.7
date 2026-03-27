// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Platform/FoxPlatformLibrary.h"
#include "Misc/EngineVersion.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "HAL/PlatformProcess.h"
#include "RHI.h"
#include "Framework/Application/SlateApplication.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

bool UFoxPlatformLibrary::IsPackagedBuild()
{
#if UE_BUILD_SHIPPING
	return true;
#else
	return !GIsEditor;
#endif
}

void UFoxPlatformLibrary::CopyToClipboard(const FString& Text)
{
#if PLATFORM_WINDOWS
	if (OpenClipboard(nullptr))
	{
		EmptyClipboard();
		const int32 Len = Text.Len() + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, Len * sizeof(TCHAR));
		if (hMem)
		{
			TCHAR* Buffer = static_cast<TCHAR*>(GlobalLock(hMem));
			FCString::Strncpy(Buffer, *Text, Len);
			GlobalUnlock(hMem);
			SetClipboardData(CF_UNICODETEXT, hMem);
		}
		CloseClipboard();
	}
#else
	UE_LOG(LogTemp, Warning, TEXT("FoxUtils: CopyToClipboard is only supported on Windows."));
#endif
}

void UFoxPlatformLibrary::OpenURLInBrowser(const FString& URL)
{
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

FString UFoxPlatformLibrary::GetEngineVersionString()
{
	return FEngineVersion::Current().ToString();
}

FString UFoxPlatformLibrary::GetPlatformName()
{
	return FPlatformProperties::PlatformName();
}

FString UFoxPlatformLibrary::GetGPUName()
{
	return GRHIAdapterName;
}

int32 UFoxPlatformLibrary::GetTotalRAM_MB()
{
	FPlatformMemoryStats Stats = FPlatformMemory::GetStats();
	return (int32)(Stats.TotalPhysical / (1024 * 1024));
}

int32 UFoxPlatformLibrary::GetTotalVRAM_MB()
{
	FTextureMemoryStats TexMemStats;
	RHIGetTextureMemoryStats(TexMemStats);
	return (int32)(TexMemStats.DedicatedVideoMemory / (1024 * 1024));
}

FString UFoxPlatformLibrary::GetCPUBrand()
{
	return FPlatformMisc::GetCPUBrand();
}

FIntPoint UFoxPlatformLibrary::GetDesktopResolution()
{
	if (FSlateApplication::IsInitialized())
	{
		FDisplayMetrics Metrics;
		FSlateApplication::Get().GetDisplayMetrics(Metrics);
		return FIntPoint(Metrics.PrimaryDisplayWidth, Metrics.PrimaryDisplayHeight);
	}
	return FIntPoint(1920, 1080);
}
