// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/RingBuffer.h"
#include "Rewind/Pawn/EnemyCharacter.h"
#include "PawnRewindComponent.generated.h"

class ARewindGameMode;
class UCapsuleComponent;

struct FPawnSavePoint{
	FVector Velocity;
	FTransform Transform;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REWIND_API UPawnRewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPawnRewindComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FPawnSavePoint CurrentSavePoint;

protected:
	virtual void BeginPlay() override;
	
private:
	AEnemyCharacter* Character;
	ARewindGameMode* GameMode;
	UCapsuleComponent* Capsule;

	UFUNCTION()
	// Called by game mode, start the rewind process
	void StartRewind();
	UFUNCTION()
	// Called by game mode, end the rewind process
	void StopRewind();

	// Manage the rewind Prossess
	void Rewind(float DeltaTime);
	UFUNCTION()
	void Record(float DeltaTime); //Record Object Transform Linear and Angular physics Velocity

	void DrawTimeLine(); // Draw the save points

	bool Rewinding = false;

	TRingBuffer<FPawnSavePoint> SavePoints;
	
	float NotSaveSince = 0.f;
	int LastReachedSavePoint = -1; 
	float RewindCurentTime = 0.f;


		
};
