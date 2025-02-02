// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RewindGameMode.generated.h"

class ARewindCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalRewindStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalRewindEnded);

UCLASS(minimalapi)
class ARewindGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARewindGameMode();

	UFUNCTION(BlueprintCallable)
	void StartGlobalRewind();
	UFUNCTION(BlueprintCallable)
	void EndGlobalRewind();
	
	FOnGlobalRewindStarted OnGlobalRewindStarted;
	FOnGlobalRewindEnded OnGlobalRewindEnded;

	bool Rewinding = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind")
	float RewindSpeed = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind", Meta = (AllowPrivateAccess))
	float RecordMaxTime = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind", Meta = (AllowPrivateAccess))
	float SavePointPerSecond = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Rewind", Meta = (AllowPrivateAccess))
	bool ShowTimeLine = false;
	UPROPERTY(BlueprintReadOnly, Category = "Rewind")
	float RemainingRewindTime = 0.f;
private:
	ARewindCharacter* Player;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
};
