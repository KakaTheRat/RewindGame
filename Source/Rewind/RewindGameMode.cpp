// Copyright Epic Games, Inc. All Rights Reserved.

#include "RewindGameMode.h"
#include "RewindCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARewindGameMode::ARewindGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ARewindGameMode::StartGlobalRewind()
{
	Rewinding = true;
	Player->CanMove= false;
	OnGlobalRewindStarted.Broadcast();
}

void ARewindGameMode::EndGlobalRewind()
{
	Rewinding = false;
	Player->CanMove= true;
	OnGlobalRewindEnded.Broadcast();
}

void ARewindGameMode::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ARewindCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}
