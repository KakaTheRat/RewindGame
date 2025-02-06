// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "Rewind/Component/PawnRewindComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnRewindComponent = CreateDefaultSubobject<UPawnRewindComponent>(TEXT("PawnRewindComponent"));

}

FVector AEnemyCharacter::GetRewindVelocity()
{
	return PawnRewindComponent->CurrentSavePoint.Velocity;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	AI = Cast<AAIController>(GetController());
	Player = Cast<ARewindCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	if (AI &&  Player)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, "Player Movement");
		AI->MoveToActor(Player);
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

