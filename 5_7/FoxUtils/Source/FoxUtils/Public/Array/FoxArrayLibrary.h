// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "FoxArrayLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxArrayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Shuffle an integer array randomly (Fisher-Yates) */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Array", meta = (DisplayName = "Shuffle Int Array"))
	static TArray<int32> ShuffleIntArray(const TArray<int32>& Array);

	/** Shuffle a string array randomly */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Array", meta = (DisplayName = "Shuffle String Array"))
	static TArray<FString> ShuffleStringArray(const TArray<FString>& Array);

	/** Remove duplicate integers from array */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Array", meta = (DisplayName = "Remove Duplicates (Int)"))
	static TArray<int32> RemoveDuplicateInts(const TArray<int32>& Array);

	/** Remove duplicate strings from array */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Array", meta = (DisplayName = "Remove Duplicates (String)"))
	static TArray<FString> RemoveDuplicateStrings(const TArray<FString>& Array);

	/** Get a random element from int array. Optionally removes it. */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Array", meta = (DisplayName = "Random Element (Int)"))
	static int32 RandomIntElement(UPARAM(ref) TArray<int32>& Array, bool bRemove, bool& bSuccess);

	/** Get N random unique elements from int array */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Array", meta = (DisplayName = "Random Elements (Int)"))
	static TArray<int32> RandomIntElements(const TArray<int32>& Array, int32 Count);

	/** Sort an integer array ascending or descending */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Array", meta = (DisplayName = "Sort Int Array"))
	static TArray<int32> SortIntArray(const TArray<int32>& Array, bool bDescending = false);

	/** Sort a float array ascending or descending */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Array", meta = (DisplayName = "Sort Float Array"))
	static TArray<float> SortFloatArray(const TArray<float>& Array, bool bDescending = false);

	/** Sort a string array alphabetically */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Array", meta = (DisplayName = "Sort String Array"))
	static TArray<FString> SortStringArray(const TArray<FString>& Array, bool bDescending = false);

	/** Weighted random selection: given Weights array, returns random index biased by weight values */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Array", meta = (DisplayName = "Weighted Random Index"))
	static int32 WeightedRandomIndex(const TArray<float>& Weights);
};
