// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Rewind/RewindCharacter.h"
#include "EnemyCharacter.generated.h"

class UAISenseConfig_Sight;
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

	void ChangePerceptionStatus(bool Activated);
	
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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetPlayerSeen() const {return PlayerSeen;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FTransform GetWeaponSocketTransform() const {return tWeaponSocket;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FRotator GetShoulderOffsetRotation() {return ShoulderRotationOffset;}

	FORCEINLINE FTimerHandle GetTimer() const {return MoveToCheckpointTimerHandle ;}

	FORCEINLINE UCameraComponent* GetCamera() const {return FollowCamera;}

	FORCEINLINE void ChangeNiagaraActivation(bool bActivate){if(bActivate){RewindNiagara->Activate();}else{RewindNiagara->Deactivate();}}

	void MoveToCheckpoint();
	void StartNextPointTimer(float Delay);
	void CancelMoveTo();


private:
	ARewindCharacter* Player;
	AAIController* AI;
	
	UPawnRewindComponent* PawnRewindComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Spline", Meta = (AllowPrivateAccess))
	USplineComponent* Spline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Sense", Meta = (AllowPrivateAccess))
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess))
	USkeletalMeshComponent* Weapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* RewindNiagara;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* ShootNiagara;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* ShootSound;
	
	FTransform tWeaponSocket;
	FRotator ShoulderRotationOffset;

	void OnSplinePointReach(FAIRequestID RequestId, const FPathFollowingResult& Result);
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	int CurrentSplinePoint = 1;
	
	FTimerHandle MoveToCheckpointTimerHandle;
	FTimerHandle ShootTimerHandle;
	FTimerHandle EndGameTimer;

	void LookAtActor(AActor* Actor);
	bool PlayerSeen = false;

	void Shoot();
	void CallBlackScreen();
};
