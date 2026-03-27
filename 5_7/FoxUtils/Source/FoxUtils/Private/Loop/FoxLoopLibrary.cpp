// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Loop/FoxLoopLibrary.h"
#include "Engine/World.h"

void UFoxLoopLibrary::ForLoopWithDelay(UObject* WorldContext, int32 FirstIndex, int32 LastIndex, float DelayPerStep,
	EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	FLatentActionManager& LatentManager = World->GetLatentActionManager();
	if (LatentManager.FindExistingAction<FFoxDelayedLoopAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
	{
		FFoxDelayedLoopAction* Action = new FFoxDelayedLoopAction(
			FirstIndex, LastIndex, 1, 1, FMath::Max(0.001f, DelayPerStep),
			Index, Outputs, LatentInfo);
		LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
	}
}

void UFoxLoopLibrary::ForEachWithDelay(UObject* WorldContext, int32 ArrayLength, float DelayPerStep,
	EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo)
{
	ForLoopWithDelay(WorldContext, 0, FMath::Max(0, ArrayLength - 1), DelayPerStep, Outputs, Index, LatentInfo);
}

void UFoxLoopLibrary::BatchedLoopWithDelay(UObject* WorldContext, int32 FirstIndex, int32 LastIndex, int32 BatchSize, float DelayPerBatch,
	EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	FLatentActionManager& LatentManager = World->GetLatentActionManager();
	if (LatentManager.FindExistingAction<FFoxDelayedLoopAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
	{
		FFoxDelayedLoopAction* Action = new FFoxDelayedLoopAction(
			FirstIndex, LastIndex, 1, FMath::Max(1, BatchSize), FMath::Max(0.001f, DelayPerBatch),
			Index, Outputs, LatentInfo);
		LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
	}
}

void UFoxLoopLibrary::ForLoopWithDelayReversed(UObject* WorldContext, int32 FirstIndex, int32 LastIndex, float DelayPerStep,
	EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	FLatentActionManager& LatentManager = World->GetLatentActionManager();
	if (LatentManager.FindExistingAction<FFoxDelayedLoopAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
	{
		// Reversed: start from LastIndex, step -1, end at FirstIndex
		FFoxDelayedLoopAction* Action = new FFoxDelayedLoopAction(
			LastIndex, FirstIndex, -1, 1, FMath::Max(0.001f, DelayPerStep),
			Index, Outputs, LatentInfo);
		LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
	}
}

void UFoxLoopLibrary::ForEachWithDelayReversed(UObject* WorldContext, int32 ArrayLength, float DelayPerStep,
	EFoxLoopOutput& Outputs, int32& Index, FLatentActionInfo LatentInfo)
{
	ForLoopWithDelayReversed(WorldContext, 0, FMath::Max(0, ArrayLength - 1), DelayPerStep, Outputs, Index, LatentInfo);
}

void UFoxLoopLibrary::CancelDelayedLoop(UObject* WorldContext, int32 UUID)
{
	if (!WorldContext) return;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return;

	World->GetLatentActionManager().RemoveActionsForObject(WorldContext);
}
