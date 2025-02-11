// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Rewind/RewindCharacter.h"
#include "AIController.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Components/SplineComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Controller.h"
#include "EnemyCharacter.generated.h"

class UPawnRewindComponent;
class AAIController;

UCLASS()
class REWIND_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation")
	bool IsRewinding = false;

	UFUNCTION(BlueprintCallable)
	FVector GetRewindVelocity();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE int GetCurrentSplinePoint() const {return CurrentSplinePoint;}
	FORCEINLINE void SetCurrentSplinePoint(const int SplinePoint) {CurrentSplinePoint = SplinePoint;}

	FORCEINLINE FTimerHandle GetTimer() const {return MoveToCheckpointTimerHandle ;}

	void MoveToCheckpoint();
	void StartNextPointTimer(float Delay);
	void CancelMoveTo();


private:
	ARewindCharacter* Player;
	AAIController* AI;
	
	UPawnRewindComponent* PawnRewindComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", Meta = (AllowPrivateAccess))
	USplineComponent* Spline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", Meta = (AllowPrivateAccess))
	UStaticMeshComponent* Rifle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", Meta = (AllowPrivateAccess))
	UStaticMeshComponent* Mag;

	void OnSplinePointReach(FAIRequestID RequestId, const FPathFollowingResult& Result);
	int CurrentSplinePoint = 1;
	FTimerHandle MoveToCheckpointTimerHandle;
};
