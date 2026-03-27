// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxInventoryLibrary.generated.h"

/**
 * Simple struct for weight calculation: an item with a weight and count.
 */
USTRUCT(BlueprintType)
struct FFoxWeightEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoxUtils")
	float Weight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoxUtils")
	int32 Count = 1;
};

UCLASS()
class FOXUTILS_API UFoxInventoryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * Split a total quantity fairly across N stacks.
	 * E.g. Total=10, Stacks=3 → [4, 3, 3]
	 */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Inventory", meta = (DisplayName = "Calculate Stack Split"))
	static TArray<int32> CalculateStackSplit(int32 TotalCount, int32 NumStacks);

	/**
	 * Check if items can stack: same type (by Name match) and current + add <= max stack.
	 */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Inventory", meta = (DisplayName = "Can Stack Items"))
	static bool CanStackItems(FName ItemTypeA, int32 CurrentCountA, FName ItemTypeB, int32 AddCount, int32 MaxStackSize);

	/**
	 * Calculate total weight from an array of {Weight, Count} entries.
	 */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Inventory", meta = (DisplayName = "Calculate Total Weight"))
	static float CalculateTotalWeight(const TArray<FFoxWeightEntry>& Items);

	/**
	 * Check if an item of ItemSize fits in a container with ContainerSlots total and OccupiedSlots used.
	 */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Inventory", meta = (DisplayName = "Fits in Container"))
	static bool FitsInContainer(int32 ItemSize, int32 ContainerSlots, int32 OccupiedSlots);

	/**
	 * Find best fit position for a W×H item in a grid inventory.
	 * Grid is GridWidth × GridHeight, OccupiedCells is a bool array (row-major).
	 * Returns true if found, outputs the top-left cell index.
	 */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Inventory", meta = (DisplayName = "Find Best Fit Slot (Grid)"))
	static bool FindBestFitSlot(const TArray<bool>& OccupiedCells, int32 GridWidth, int32 GridHeight,
		int32 ItemWidth, int32 ItemHeight, int32& OutCellX, int32& OutCellY);
};
