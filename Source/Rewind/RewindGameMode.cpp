// Copyright Epic Games, Inc. All Rights Reserved.

#include "RewindGameMode.h"
#include "RewindCharacter.h"
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

void ARewindGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Rewinding)
	{
		RemainingRewindTime = FMath::Clamp<float>(RemainingRewindTime - (DeltaSeconds * RewindSpeed), 0.0f, RecordMaxTime);
		if (RemainingRewindTime <= 0)
		{
			EndGlobalRewind();
		}
	}else
	{
		RemainingRewindTime = FMath::Clamp<float>(RemainingRewindTime + DeltaSeconds, 0.0f, RecordMaxTime);
	}
}
