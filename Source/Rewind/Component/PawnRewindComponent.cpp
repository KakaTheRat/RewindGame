// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnRewindComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Rewind/RewindGameMode.h"

// Sets default values for this component's properties
UPawnRewindComponent::UPawnRewindComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPawnRewindComponent::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ARewindGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnGlobalRewindStarted.AddDynamic(this, &ThisClass::StartRewind);
		GameMode->OnGlobalRewindEnded.AddDynamic(this, &ThisClass::StopRewind);
	}

	Character = Cast<AEnemyCharacter>(GetOwner());
	if (Character)
	{
		Capsule = Character->GetCapsuleComponent();
	}
	
}

void UPawnRewindComponent::StartRewind()
{
	if (Character)
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_None);
		CurrentSavePoint.Velocity = Character->GetCharacterMovement()->Velocity;
	}
	Rewinding = true;
	Character->IsRewinding = true;
}

void UPawnRewindComponent::StopRewind()
{
	Rewinding = false;
	Character->IsRewinding = false;
	if (Character)
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_NavWalking);
		Character->GetCharacterMovement()->Velocity = CurrentSavePoint.Velocity;
	}
	CurrentSavePoint.Velocity = FVector::ZeroVector;
	LastReachedSavePoint = -1;
	RewindCurentTime = 0.f;
}

void UPawnRewindComponent::Rewind(float DeltaTime)
{
	float Alpha = 0.f;
	int TargetedIndex = -1;

	if (LastReachedSavePoint == 0) return;

	RewindCurentTime += GameMode->RewindSpeed * DeltaTime;

	if (LastReachedSavePoint == -1)
	{
		float TotalTime = NotSaveSince;
		Alpha = FMath::Clamp<float>(RewindCurentTime / TotalTime, 0.f, 1.f);
		TargetedIndex = SavePoints.Num() - 1;
	}
	else
	{
		float TotalTime = 1.0f / GameMode->SavePointPerSecond;
		Alpha = FMath::Clamp<float>(RewindCurentTime / TotalTime, 0.0f, 1.0f);
		TargetedIndex = LastReachedSavePoint - 1;
	}

	FPawnSavePoint TargetedSavePoint = SavePoints[TargetedIndex];
	FTransform RewindTransform;
	RewindTransform.Blend(Capsule->GetComponentTransform(), TargetedSavePoint.Transform, Alpha);
	FVector Velocity = FMath::Lerp<FVector>(CurrentSavePoint.Velocity, TargetedSavePoint.Velocity, Alpha);
	CurrentSavePoint.Velocity = Velocity;
	Capsule->SetWorldTransform(RewindTransform);

	if (Alpha >= 1.0f)
	{
		if (LastReachedSavePoint == -1)
		{
			LastReachedSavePoint = SavePoints.Num() - 1;
		}
		else
		{
			LastReachedSavePoint -= 1;
			if (SavePoints.Num() - 1 - LastReachedSavePoint >= 1)
			{
				SavePoints.Pop();
			}
		}
		RewindCurentTime = 0.f;
	}
}

void UPawnRewindComponent::Record(float DeltaTime)
{
	int MaxRecordPoint = GameMode->RecordMaxTime * GameMode->SavePointPerSecond;
	if (SavePoints.Num() - 1 > MaxRecordPoint){SavePoints.PopFront();}
	float SaveTime = 1.f/GameMode->SavePointPerSecond;
	if (NotSaveSince >= SaveTime)
	{
		FTransform Transform = Capsule->GetComponentTransform();
		FPawnSavePoint SavePoint;
		SavePoint.Velocity = Character->GetCharacterMovement()->Velocity;
		SavePoint.Transform = Transform;
		SavePoints.Add(SavePoint);
		NotSaveSince = 0.f;
	}else
	{
		NotSaveSince += DeltaTime;
	}
}

void UPawnRewindComponent::DrawTimeLine()
{
	for (int i = 0; i < SavePoints.Num(); i++)
	{
		DrawDebugPoint(GetWorld(), SavePoints[i].Transform.GetLocation(), 10, FColor::Green);
		if (i!=0)
		{
			DrawDebugLine(GetWorld(), SavePoints[i].Transform.GetLocation(), SavePoints[i-1].Transform.GetLocation(), FColor::Green);
		}
	}
}


// Called every frame
void UPawnRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Rewinding){Rewind(DeltaTime);}
	else{Record(DeltaTime);}

	if (GameMode->ShowTimeLine)
	{
		DrawTimeLine();
	}
}

