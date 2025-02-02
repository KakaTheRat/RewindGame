// Fill out your copyright notice in the Description page of Project Settings.


#include "RewindComponent.h"
#include "Rewind/RewindGameMode.h"
#include "TimerManager.h"



// Sets default values for this component's properties
URewindComponent::URewindComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URewindComponent::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ARewindGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnGlobalRewindStarted.AddDynamic(this, &ThisClass::URewindComponent::StartRewind);
		GameMode->OnGlobalRewindEnded.AddDynamic(this, &ThisClass::URewindComponent::StopRewind);
	}

	RewindableObject->SetSimulatePhysics(true);
}

void URewindComponent::StartRewind()
{
	RewindCurrentVelocities.LinearVelocity = RewindableObject->GetPhysicsLinearVelocity();
	RewindCurrentVelocities.AngularVelocity = RewindableObject->GetPhysicsAngularVelocityInDegrees();
	RewindableObject->SetSimulatePhysics(false);
	Rewinding = true;
}

void URewindComponent::StopRewind()
{
	if (!Rewinding){return;}
	RewindableObject->SetPhysicsLinearVelocity(RewindCurrentVelocities.LinearVelocity);
	RewindableObject->SetPhysicsAngularVelocityInDegrees(RewindCurrentVelocities.AngularVelocity);
	RewindableObject->SetSimulatePhysics(true);
	LastReachedSavePoint = -1;
	RewindCurentTime = 0.f;
	Rewinding = false;
}

void URewindComponent::Rewind(float DeltaTime)
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

	FSavePoint TargetedSavePoint = SavePoints[TargetedIndex];
	FTransform RewindTransform;
	RewindTransform.Blend(RewindableObject->GetComponentTransform(), TargetedSavePoint.Transform, Alpha);
	FVector LinearVelocity = FMath::Lerp<FVector>(RewindCurrentVelocities.LinearVelocity, TargetedSavePoint.Velocities.LinearVelocity, Alpha);
	FVector AngularVelocity = FMath::Lerp<FVector>(RewindCurrentVelocities.AngularVelocity, TargetedSavePoint.Velocities.AngularVelocity, Alpha);
	RewindCurrentVelocities.LinearVelocity = LinearVelocity;
	RewindCurrentVelocities.AngularVelocity = AngularVelocity;
	RewindableObject->SetWorldTransform(RewindTransform);

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

void URewindComponent::Record(float DeltaTime)
{
	int MaxRecordPoint = GameMode->RecordMaxTime * GameMode->SavePointPerSecond;
	if (SavePoints.Num() > MaxRecordPoint){SavePoints.PopFront();}
	float SaveTime = 1.f/GameMode->SavePointPerSecond;
	if (NotSaveSince >= SaveTime)
	{
		FTransform Transform = RewindableObject->GetComponentTransform();
		FVector LinearVelocity = RewindableObject->GetPhysicsLinearVelocity();
		FVector AngularVelocity = RewindableObject->GetPhysicsAngularVelocityInDegrees();
		FSavePoint SavePoint;
		SavePoint.Velocities.LinearVelocity = LinearVelocity;
		SavePoint.Velocities.AngularVelocity = AngularVelocity;
		SavePoint.Transform = Transform;
		SavePoints.Add(SavePoint);
		NotSaveSince = 0.f;
	}else
	{
		NotSaveSince += DeltaTime;
	}
}

void URewindComponent::DrawTimeLine()
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
void URewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Rewinding){Rewind(DeltaTime);}
	else{Record(DeltaTime);}

	if (GameMode->ShowTimeLine)
	{
		DrawTimeLine();
	}
	
	
}

void URewindComponent::SetRewindableObject(UStaticMeshComponent* StaticMesh)
{
	RewindableObject = StaticMesh;
}

