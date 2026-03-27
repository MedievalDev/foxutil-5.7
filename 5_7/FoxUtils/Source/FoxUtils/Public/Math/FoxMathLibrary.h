// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FoxMathLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Remap a value from one range to another. InputValue in [InMin,InMax] → result in [OutMin,OutMax] */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Remap Range"))
	static float RemapRange(float Value, float InMin, float InMax, float OutMin, float OutMax, bool bClamp = true);

	/** Round a float to N decimal places: 3.14159 with 2 → 3.14 */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Round to Decimals"))
	static float RoundToDecimals(float Value, int32 Decimals);

	/** Normalize angle to 0-360 or -180 to 180 range */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Normalize Angle"))
	static float NormalizeAngle(float Angle, bool bSigned = false);

	/** 2D distance between two vectors (ignores Z) */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Distance 2D"))
	static float Distance2D(FVector A, FVector B);

	/** Float equality with configurable tolerance */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Is Nearly Equal (Float)"))
	static bool IsNearlyEqualFloat(float A, float B, float Tolerance = 0.0001f);

	/** Returns -1, 0, or 1 based on sign of value */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Sign (Float)"))
	static int32 SignFloat(float Value);

	/** Wrap value into range [Min, Max). Works correctly with negative values unlike % operator */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Wrap (Float)"))
	static float WrapFloat(float Value, float Min, float Max);

	/** "A is X% of B" — safe division, returns 0 if B is zero */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Math", meta = (DisplayName = "Percentage"))
	static float Percentage(float A, float B);
};
