// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Rewind/Component/PawnRewindComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnRewindComponent = CreateDefaultSubobject<UPawnRewindComponent>(TEXT("PawnRewindComponent"));

	Rifle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rifle"));
	Rifle->SetupAttachment(GetMesh(), TEXT("Weapon"));
	
	Mag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mag"));
	Mag->SetupAttachment(Rifle);

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SPline"));

}

FVector AEnemyCharacter::GetRewindVelocity()
{
	return PawnRewindComponent->CurrentSavePoint.Velocity;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetController()->UnPossess();

	AI = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(),GetActorLocation(),GetActorRotation());
	AI->Possess(this);
	
	Player = Cast<ARewindCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	if (AI &&  Player)
	{
		AI->MoveToLocation(Spline->GetLocationAtSplinePoint(0,ESplineCoordinateSpace::World), 50.F);
		AI->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemyCharacter::OnSPlinePointReach);
	}
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::OnSPlinePointReach(FAIRequestID RequestId, const FPathFollowingResult& Result)
{
	if(CurrentSplinePoint < Spline->GetNumberOfSplinePoints() - 1)
	{
		CurrentSplinePoint++;
	}
	else
	{
		CurrentSplinePoint = 0;
	}
	AI->MoveToLocation(Spline->GetLocationAtSplinePoint(CurrentSplinePoint,ESplineCoordinateSpace::World));
	bool Loop = Spline->IsClosedLoop();
	
}

