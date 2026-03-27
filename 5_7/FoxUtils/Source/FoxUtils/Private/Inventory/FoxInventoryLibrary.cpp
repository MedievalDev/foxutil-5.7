// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Inventory/FoxInventoryLibrary.h"

TArray<int32> UFoxInventoryLibrary::CalculateStackSplit(int32 TotalCount, int32 NumStacks)
{
	TArray<int32> Result;
	if (NumStacks <= 0 || TotalCount <= 0) return Result;

	int32 Base = TotalCount / NumStacks;
	int32 Remainder = TotalCount % NumStacks;

	Result.Reserve(NumStacks);
	for (int32 i = 0; i < NumStacks; ++i)
	{
		// Distribute remainder across first stacks
		Result.Add(Base + (i < Remainder ? 1 : 0));
	}

	return Result;
}

bool UFoxInventoryLibrary::CanStackItems(FName ItemTypeA, int32 CurrentCountA, FName ItemTypeB, int32 AddCount, int32 MaxStackSize)
{
	if (ItemTypeA != ItemTypeB) return false;
	return (CurrentCountA + AddCount) <= MaxStackSize;
}

float UFoxInventoryLibrary::CalculateTotalWeight(const TArray<FFoxWeightEntry>& Items)
{
	float Total = 0.f;
	for (const FFoxWeightEntry& Entry : Items)
	{
		Total += Entry.Weight * (float)Entry.Count;
	}
	return Total;
}

bool UFoxInventoryLibrary::FitsInContainer(int32 ItemSize, int32 ContainerSlots, int32 OccupiedSlots)
{
	return (OccupiedSlots + ItemSize) <= ContainerSlots;
}

bool UFoxInventoryLibrary::FindBestFitSlot(const TArray<bool>& OccupiedCells, int32 GridWidth, int32 GridHeight,
	int32 ItemWidth, int32 ItemHeight, int32& OutCellX, int32& OutCellY)
{
	OutCellX = -1;
	OutCellY = -1;

	if (OccupiedCells.Num() != GridWidth * GridHeight) return false;
	if (ItemWidth <= 0 || ItemHeight <= 0) return false;
	if (ItemWidth > GridWidth || ItemHeight > GridHeight) return false;

	// Scan top-left to bottom-right for first fit
	for (int32 Y = 0; Y <= GridHeight - ItemHeight; ++Y)
	{
		for (int32 X = 0; X <= GridWidth - ItemWidth; ++X)
		{
			bool bFits = true;
			for (int32 dy = 0; dy < ItemHeight && bFits; ++dy)
			{
				for (int32 dx = 0; dx < ItemWidth && bFits; ++dx)
				{
					int32 CellIdx = (Y + dy) * GridWidth + (X + dx);
					if (OccupiedCells[CellIdx])
					{
						bFits = false;
					}
				}
			}

			if (bFits)
			{
				OutCellX = X;
				OutCellY = Y;
				return true;
			}
		}
	}

	return false;
}
