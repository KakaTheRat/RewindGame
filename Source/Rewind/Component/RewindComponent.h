#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/RingBuffer.h"
#include "RewindComponent.generated.h"

class ARewindGameMode;

struct FVelocities
{
	FVector LinearVelocity;
	FVector AngularVelocity;
};

struct FSavePoint
{
	FTransform Transform;
	FVelocities Velocities;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REWIND_API URewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URewindComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetRewindableObject(UStaticMeshComponent* StaticMesh);

protected:
	virtual void BeginPlay() override;

private:
	//Object that can be rewind
	UStaticMeshComponent* RewindableObject;

	UFUNCTION()
	// Called by game mode, start the rewind process
	void StartRewind();
	UFUNCTION()
	// Called by game mode, end the rewind process
	void StopRewind();

	// Manage the rewind Prossess
	void Rewind(float DeltaTime);
	void Record(float DeltaTime); //Record Object Transform Linear and Angular physics Velocity

	void DrawTimeLine(); // Draw the save points

	bool Rewinding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind", Meta = (AllowPrivateAccess))
	float RecordMaxTime = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewind", Meta = (AllowPrivateAccess))
	float SavePointPerSecond = 60.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Rewind", Meta = (AllowPrivateAccess))
	bool ShowTimeLine = false;

	TRingBuffer<FSavePoint> SavePoints;
	FVelocities RewindCurrentVelocities;
	
	float NotSaveSince = 0.f; //Time since when there is no new save point

	int LastReachedSavePoint = -1; 
	float RewindCurentTime = 0.f; //Time sice when the rewind havn't reach a save point

	ARewindGameMode* GameMode;
};
