// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxSaveLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxSaveLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Save a string value to INI file under Section/Key */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Save", meta = (DisplayName = "Save String to INI"))
	static void SaveStringToINI(const FString& Section, const FString& Key, const FString& Value, const FString& FileName = TEXT("FoxUtils"));

	/** Load a string value from INI file. Returns Default if not found. */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Save", meta = (DisplayName = "Load String from INI"))
	static FString LoadStringFromINI(const FString& Section, const FString& Key, const FString& Default = TEXT(""), const FString& FileName = TEXT("FoxUtils"));

	/** Save an integer to INI */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Save", meta = (DisplayName = "Save Int to INI"))
	static void SaveIntToINI(const FString& Section, const FString& Key, int32 Value, const FString& FileName = TEXT("FoxUtils"));

	/** Load an integer from INI. Returns Default if not found. */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Save", meta = (DisplayName = "Load Int from INI"))
	static int32 LoadIntFromINI(const FString& Section, const FString& Key, int32 Default = 0, const FString& FileName = TEXT("FoxUtils"));

	/** Save a float to INI */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Save", meta = (DisplayName = "Save Float to INI"))
	static void SaveFloatToINI(const FString& Section, const FString& Key, float Value, const FString& FileName = TEXT("FoxUtils"));

	/** Load a float from INI. Returns Default if not found. */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Save", meta = (DisplayName = "Load Float from INI"))
	static float LoadFloatFromINI(const FString& Section, const FString& Key, float Default = 0.f, const FString& FileName = TEXT("FoxUtils"));
};
