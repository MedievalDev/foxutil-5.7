// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxPlatformLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxPlatformLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Is Packaged Build"))
	static bool IsPackagedBuild();

	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Platform", meta = (DisplayName = "Copy to Clipboard"))
	static void CopyToClipboard(const FString& Text);

	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Platform", meta = (DisplayName = "Open URL in Browser"))
	static void OpenURLInBrowser(const FString& URL);

	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Get Engine Version"))
	static FString GetEngineVersionString();

	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Get Platform Name"))
	static FString GetPlatformName();

	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Get GPU Name"))
	static FString GetGPUName();

	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Get Total RAM (MB)"))
	static int32 GetTotalRAM_MB();

	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Get Total VRAM (MB)"))
	static int32 GetTotalVRAM_MB();

	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Get CPU Brand"))
	static FString GetCPUBrand();

	UFUNCTION(BlueprintPure, Category = "FoxUtils|Platform", meta = (DisplayName = "Get Desktop Resolution"))
	static FIntPoint GetDesktopResolution();
};
