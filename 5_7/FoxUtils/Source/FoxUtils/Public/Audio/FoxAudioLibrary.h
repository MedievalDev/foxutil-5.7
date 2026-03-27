// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "FoxAudioLibrary.generated.h"

UCLASS()
class FOXUTILS_API UFoxAudioLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Play sound at location with volume, pitch, and optional attenuation — all in one node */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Audio", meta = (WorldContext = "WorldContext", DisplayName = "Play Sound Extended", AdvancedDisplay = "3"))
	static UAudioComponent* PlaySoundExtended(UObject* WorldContext, USoundBase* Sound, FVector Location, float Volume = 1.f,
		float Pitch = 1.f, float StartTime = 0.f, USoundAttenuation* Attenuation = nullptr);

	/** Play a random sound from an array */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Audio", meta = (WorldContext = "WorldContext", DisplayName = "Play Random Sound"))
	static UAudioComponent* PlayRandomSound(UObject* WorldContext, const TArray<USoundBase*>& Sounds, FVector Location, float Volume = 1.f, float Pitch = 1.f);

	/** Fade an audio component's volume over time */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Audio", meta = (DisplayName = "Fade Sound Volume"))
	static void FadeSoundVolume(UAudioComponent* AudioComponent, float TargetVolume, float FadeDuration);

	/** Stop all audio components on an actor */
	UFUNCTION(BlueprintCallable, Category = "FoxUtils|Audio", meta = (DisplayName = "Stop All Sounds on Actor"))
	static void StopAllSoundsOnActor(AActor* Actor);

	/** Check if any audio component on actor is currently playing */
	UFUNCTION(BlueprintPure, Category = "FoxUtils|Audio", meta = (DisplayName = "Is Any Sound Playing"))
	static bool IsAnySoundPlaying(AActor* Actor);
};
