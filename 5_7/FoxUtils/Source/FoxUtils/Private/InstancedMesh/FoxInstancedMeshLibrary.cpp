// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "InstancedMesh/FoxInstancedMeshLibrary.h"
#include "FoliageInstancedStaticMeshComponent.h"

void UFoxInstancedMeshLibrary::HISMRefreshCustomData(UHierarchicalInstancedStaticMeshComponent* HISM, bool bAsync, bool bForceUpdate)
{
	if (!HISM) return;
	HISM->BuildTreeIfOutdated(bAsync, bForceUpdate);
}

void UFoxInstancedMeshLibrary::FISMRefreshCustomData(UInstancedStaticMeshComponent* ISM, bool bAsync, bool bForceUpdate)
{
	if (!ISM) return;
	// UFoliageInstancedStaticMeshComponent inherits from HISM which has BuildTreeIfOutdated
	UHierarchicalInstancedStaticMeshComponent* HISM = Cast<UHierarchicalInstancedStaticMeshComponent>(ISM);
	if (HISM)
	{
		HISM->BuildTreeIfOutdated(bAsync, bForceUpdate);
	}
}

TArray<int32> UFoxInstancedMeshLibrary::HISMGetInstancesInRadius(UHierarchicalInstancedStaticMeshComponent* HISM, FVector Location, float Radius)
{
	TArray<int32> Result;
	if (!HISM) return Result;

	float RadSq = Radius * Radius;
	int32 Count = HISM->GetInstanceCount();

	for (int32 i = 0; i < Count; ++i)
	{
		FTransform InstanceTransform;
		if (HISM->GetInstanceTransform(i, InstanceTransform, true))
		{
			if (FVector::DistSquared(InstanceTransform.GetLocation(), Location) <= RadSq)
			{
				Result.Add(i);
			}
		}
	}

	return Result;
}

int32 UFoxInstancedMeshLibrary::HISMGetNearestInstance(UHierarchicalInstancedStaticMeshComponent* HISM, FVector Location, FTransform& OutTransform)
{
	if (!HISM) return -1;

	int32 Count = HISM->GetInstanceCount();
	int32 NearestIdx = -1;
	float NearestDistSq = MAX_FLT;

	for (int32 i = 0; i < Count; ++i)
	{
		FTransform InstanceTransform;
		if (HISM->GetInstanceTransform(i, InstanceTransform, true))
		{
			float DistSq = FVector::DistSquared(InstanceTransform.GetLocation(), Location);
			if (DistSq < NearestDistSq)
			{
				NearestDistSq = DistSq;
				NearestIdx = i;
				OutTransform = InstanceTransform;
			}
		}
	}

	return NearestIdx;
}

TArray<int32> UFoxInstancedMeshLibrary::HISMBatchAddInstances(UHierarchicalInstancedStaticMeshComponent* HISM, const TArray<FTransform>& Transforms, bool bWorldSpace)
{
	TArray<int32> Indices;
	if (!HISM) return Indices;

	Indices.Reserve(Transforms.Num());
	for (const FTransform& T : Transforms)
	{
		int32 Idx;
		Idx = HISM->AddInstance(T, bWorldSpace);
		Indices.Add(Idx);
	}

	return Indices;
}

void UFoxInstancedMeshLibrary::HISMBatchUpdateCustomData(UHierarchicalInstancedStaticMeshComponent* HISM, const TArray<int32>& Indices, int32 CustomDataIndex, float Value)
{
	if (!HISM) return;

	for (int32 Idx : Indices)
	{
		HISM->SetCustomDataValue(Idx, CustomDataIndex, Value, false); // bMarkRenderStateDirty = false for batch
	}

	HISM->MarkRenderStateDirty();
}
