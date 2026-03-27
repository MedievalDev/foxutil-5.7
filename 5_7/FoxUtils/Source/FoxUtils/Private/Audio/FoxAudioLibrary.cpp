// Copyright 2026 Alchemy Fox Studio. All Rights Reserved.
#include "Audio/FoxAudioLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

UAudioComponent* UFoxAudioLibrary::PlaySoundExtended(UObject* WorldContext, USoundBase* Sound, FVector Location, float Volume,
	float Pitch, float StartTime, USoundAttenuation* Attenuation)
{
	if (!WorldContext || !Sound) return nullptr;

	UAudioComponent* AC = UGameplayStatics::SpawnSoundAtLocation(
		WorldContext, Sound, Location, FRotator::ZeroRotator, Volume, Pitch, StartTime, Attenuation);

	return AC;
}

UAudioComponent* UFoxAudioLibrary::PlayRandomSound(UObject* WorldContext, const TArray<USoundBase*>& Sounds, FVector Location, float Volume, float Pitch)
{
	if (!WorldContext || Sounds.Num() == 0) return nullptr;

	int32 Idx = FMath::RandRange(0, Sounds.Num() - 1);
	USoundBase* Sound = Sounds[Idx];
	if (!Sound) return nullptr;

	return UGameplayStatics::SpawnSoundAtLocation(WorldContext, Sound, Location, FRotator::ZeroRotator, Volume, Pitch);
}

void UFoxAudioLibrary::FadeSoundVolume(UAudioComponent* AudioComponent, float TargetVolume, float FadeDuration)
{
	if (!AudioComponent) return;

	if (FadeDuration <= 0.f)
	{
		AudioComponent->SetVolumeMultiplier(TargetVolume);
		return;
	}

	AudioComponent->AdjustVolume(FadeDuration, TargetVolume);
}

void UFoxAudioLibrary::StopAllSoundsOnActor(AActor* Actor)
{
	if (!Actor) return;

	TArray<UAudioComponent*> AudioComps;
	Actor->GetComponents<UAudioComponent>(AudioComps);

	for (UAudioComponent* AC : AudioComps)
	{
		if (AC && AC->IsPlaying())
		{
			AC->Stop();
		}
	}
}

bool UFoxAudioLibrary::IsAnySoundPlaying(AActor* Actor)
{
	if (!Actor) return false;

	TArray<UAudioComponent*> AudioComps;
	Actor->GetComponents<UAudioComponent>(AudioComps);

	for (UAudioComponent* AC : AudioComps)
	{
		if (AC && AC->IsPlaying())
		{
			return true;
		}
	}

	return false;
}
