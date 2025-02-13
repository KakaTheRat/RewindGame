// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "AIController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Rewind/RewindGameMode.h"
#include "Rewind/Component/PawnRewindComponent.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnRewindComponent = CreateDefaultSubobject<UPawnRewindComponent>(TEXT("PawnRewindComponent"));

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(),"RightHand");
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SPline"));

	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	if (UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig")))
	{
		SightConfig->DetectionByAffiliation.bDetectEnemies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	RewindNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Rewind Niagara"));
	RewindNiagara->SetupAttachment(GetMesh());
	RewindNiagara->bAutoActivate = false;

	ShootNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Shoot Niagara"));
	ShootNiagara->SetupAttachment(Weapon,TEXT("Canon"));
	ShootNiagara->bAutoActivate = false;
	ShootNiagara->SetRelativeRotation(FRotator(0.f,0.f,-90.f));
	
}

void AEnemyCharacter::ChangePerceptionStatus(bool Activated)
{
	AIPerception->SetSenseEnabled(AIPerception->GetDominantSense(), Activated);
}

FVector AEnemyCharacter::GetRewindVelocity()
{
	return PawnRewindComponent->GetCurrentSavePoint().Velocity;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetController()->UnPossess();

	AI = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(),GetActorLocation(),GetActorRotation());
	AI->Possess(this);
	
	Player = Cast<ARewindCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
	if (AI)
	{
		AI->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemyCharacter::OnSplinePointReach);
		MoveToCheckpoint();
	}

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	if(AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyCharacter::OnPerceptionUpdated);
	}

	FollowCamera->Deactivate();
	
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(PlayerSeen)
	{
		CancelMoveTo();
		LookAtActor(Player);
	}

	if(Weapon)
	{
		tWeaponSocket = Weapon->GetBoneTransform(TEXT("HandSocket"));
	}
	
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCharacter::CancelMoveTo()
{
	AI->StopMovement();
}

void AEnemyCharacter::OnSplinePointReach(FAIRequestID RequestId, const FPathFollowingResult& Result)
{
	if(Result.IsSuccess())
	{
		if(CurrentSplinePoint < Spline->GetNumberOfSplinePoints() - 1) {CurrentSplinePoint++;}
		else {CurrentSplinePoint = 0;}
		if(Spline->IsClosedLoop()) {MoveToCheckpoint();}
		else {StartNextPointTimer(5.f);}
	}else
	{
		MoveToCheckpoint();
	}
}

void AEnemyCharacter::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if(ARewindCharacter* DetectedPlayer = Cast<ARewindCharacter>(Actor))
	{
		if(Stimulus.IsValid())
		{
			CancelMoveTo();
			MoveToCheckpointTimerHandle.Invalidate();
			DetectedPlayer->DisableInputs();
			PlayerSeen = true;
			DetectedPlayer->Catched(this);
			GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &ThisClass::Shoot, 1.5, false);
		}
		
	}
}

void AEnemyCharacter::LookAtActor(AActor* Actor)
{
	FVector ToTarget = Actor->GetTargetLocation() - Weapon->GetBoneTransform("Canon").GetLocation();
	FRotator LookAtRotation =  FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	GetCapsuleComponent()->SetWorldRotation(
		FMath::RInterpTo(
		GetCapsuleComponent()->GetComponentRotation(),
		LookAtRotation,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		10));

	FRotator TargetShoulderRotation = (Player->GetTargetLocation() - Weapon->GetBoneTransform(TEXT("Canon")).GetLocation()).Rotation();
	TargetShoulderRotation = -1 * FRotator(0.f, 0.f, TargetShoulderRotation.Pitch);
	ShoulderRotationOffset = FMath::RInterpTo(
		ShoulderRotationOffset,
		TargetShoulderRotation,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		10);
}

void AEnemyCharacter::Shoot()
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.1);
	ShootNiagara->ActivateSystem();
	CallBlackScreen();
}

void AEnemyCharacter::CallBlackScreen()
{
	Player->BlackScreen();
}


void AEnemyCharacter::MoveToCheckpoint()
{
	if(PlayerSeen){return;}
	AI->MoveToLocation(Spline->GetLocationAtSplinePoint(CurrentSplinePoint,ESplineCoordinateSpace::World));
}

void AEnemyCharacter::StartNextPointTimer(float Delay)
{
	GetWorldTimerManager().SetTimer(MoveToCheckpointTimerHandle, this, &ThisClass::MoveToCheckpoint, Delay, false);
}
