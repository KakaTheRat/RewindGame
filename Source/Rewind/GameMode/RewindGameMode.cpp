// Copyright Epic Games, Inc. All Rights Reserved.

#include "RewindGameMode.h"
#include "../Pawn/RewindCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARewindGameMode::ARewindGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
}

void ARewindGameMode::StartGlobalRewind()
{
	Rewinding = true;
	Player->CanMove= false;
	OnGlobalRewindStarted.Broadcast();
	Player->SetRewindPostProcessActivation(true);
}

void ARewindGameMode::EndGlobalRewind()
{
	Rewinding = false;
	Player->CanMove= true;
	OnGlobalRewindEnded.Broadcast();
	Player->SetRewindPostProcessActivation(false);

}

void ARewindGameMode::EndGame()
{
	Player->BlackScreen();
}

void ARewindGameMode::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<ARewindCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void ARewindGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
