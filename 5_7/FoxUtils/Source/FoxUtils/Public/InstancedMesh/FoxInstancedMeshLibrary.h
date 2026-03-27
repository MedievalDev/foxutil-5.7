// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "FoxInstancedMeshLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxInstancedMeshLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Rebuild HISM tree (exposes BuildTreeIfOutdated to Blueprint) */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|InstancedMesh", meta = (DisplayName = "HISM Refresh Custom Data"))
	static void HISMRefreshCustomData(UHierarchicalInstancedStaticMeshComponent* HISM, bool bAsync = true, bool bForceUpdate = false);

	/** Rebuild Foliage ISM tree */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|InstancedMesh", meta = (DisplayName = "FISM Refresh Custom Data"))
	static void FISMRefreshCustomData(UInstancedStaticMeshComponent* ISM, bool bAsync = true, bool bForceUpdate = false);

	/** Get all instance indices within radius of a point */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|InstancedMesh", meta = (DisplayName = "HISM Get Instances in Radius"))
	static TArray<int32> HISMGetInstancesInRadius(UHierarchicalInstancedStaticMeshComponent* HISM, FVector Location, float Radius);

	/** Get the nearest instance to a point (returns index and transform) */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|InstancedMesh", meta = (DisplayName = "HISM Get Nearest Instance"))
	static int32 HISMGetNearestInstance(UHierarchicalInstancedStaticMeshComponent* HISM, FVector Location, FTransform& OutTransform);

	/** Add multiple instances at once from an array of transforms */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|InstancedMesh", meta = (DisplayName = "HISM Batch Add Instances"))
	static TArray<int32> HISMBatchAddInstances(UHierarchicalInstancedStaticMeshComponent* HISM, const TArray<FTransform>& Transforms, bool bWorldSpace = false);

	/** Set custom data float for multiple instances in one call */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|InstancedMesh", meta = (DisplayName = "HISM Batch Update Custom Data"))
	static void HISMBatchUpdateCustomData(UHierarchicalInstancedStaticMeshComponent* HISM, const TArray<int32>& Indices, int32 CustomDataIndex, float Value);
};
