// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxStringLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxStringLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Truncate string to MaxLength, appending Suffix if truncated. "Hello World" with MaxLen=7 → "Hello W..." */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Truncate String"))
	static FString TruncateString(const FString& Source, int32 MaxLength, const FString& Suffix = TEXT("..."));

	/** Case-insensitive Contains check */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Contains (Ignore Case)"))
	static bool ContainsIgnoreCase(const FString& Source, const FString& SubString);

	/** Convert to Title Case: "hello world" → "Hello World" */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "To Title Case"))
	static FString ToTitleCase(const FString& Source);

	/** Check if string is empty or contains only whitespace */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Is Empty or Whitespace"))
	static bool IsEmptyOrWhitespace(const FString& Source);

	/** Format a number with thousands separator: 1234567 → "1,234,567" */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Format Number"))
	static FString FormatNumber(int64 Number, const FString& Separator = TEXT(","));

	/** Format seconds into readable time: 125.3 → "2:05" or "02m 05s" */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Format Time Seconds"))
	static FString FormatTimeSeconds(float TotalSeconds, bool bUseLetters = false, bool bShowMilliseconds = false);

	/** Remove all whitespace (leading, trailing, and inner) */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Remove Whitespace"))
	static FString RemoveWhitespace(const FString& Source);

	/** Pad a string to MinLength. PadLeft: "7" → "007", PadRight: "7" → "700" */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Pad String"))
	static FString PadString(const FString& Source, int32 MinLength, const FString& PadChar = TEXT("0"), bool bPadLeft = true);

	/** Split string at first occurrence of delimiter. Returns true if found. */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Split First"))
	static bool SplitFirst(const FString& Source, const FString& Delimiter, FString& Left, FString& Right);

	/** Join a string array with separator: ["A","B","C"] + ", " → "A, B, C" */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|String", meta = (DisplayName = "Join String Array"))
	static FString JoinStringArray(const TArray<FString>& Array, const FString& Separator = TEXT(", "));
};
