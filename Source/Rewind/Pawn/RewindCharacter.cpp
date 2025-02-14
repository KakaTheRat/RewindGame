// Copyright Epic Games, Inc. All Rights Reserved.

#include "RewindCharacter.h"

#include <string>

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "../GameMode/RewindGameMode.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Pawn/EnemyCharacter.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARewindCharacter

ARewindCharacter::ARewindCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	RewindPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("RewindPostProcess"));
	RewindPostProcess->bEnabled = false;

	XRayPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("XRayPostProcess"));
	XRayPostProcess->bEnabled = false;
}

void ARewindCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	DisableInputs();
}

void ARewindCharacter::LewerTurnedOn()
{
	LewerOn += 1;
}

void ARewindCharacter::Catched(AActor* CatchedBy)
{
	Catch = true;
	Catcher = CatchedBy;
	ChangeCamSmoothly(Catcher);
	GetWorldTimerManager().SetTimer(RestartGameTimer, this, &ThisClass::StartRewindAnim, 1.48, false);
}

void ARewindCharacter::ChangeCamSmoothly(AActor* Actor)
{
	if(AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Actor))
	{
		Enemy->GetCamera()->Activate();
	}
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetViewTargetWithBlend(Actor, 1.F);
	}
}

void ARewindCharacter::LookAtCatcher()
{
	if(!Catcher) {return;}
	FVector ToTarget = Catcher->GetTargetLocation() - GetCapsuleComponent()->GetComponentLocation();
	FRotator LookAtRotation =  FRotator(0.f, ToTarget.Rotation().Yaw, 0.f);
	GetCapsuleComponent()->SetWorldRotation(
		FMath::RInterpTo(
		GetCapsuleComponent()->GetComponentRotation(),
		LookAtRotation,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		10));
}

void ARewindCharacter::EndGame()
{
	UGameplayStatics::OpenLevel(this, FName(GetWorld()->GetMapName()));
}

void ARewindCharacter::BlackScreen()
{
	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			CameraManager->StartCameraFade(0.f, 1.f, 0.3f, FColor::Black, false, true);
			GetWorldTimerManager().SetTimer(RestartGameTimer, this, &ThisClass::EndGame, 0.3f, false);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARewindCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ARewindCharacter::Jump);
		//	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARewindCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARewindCharacter::Look);

		EnhancedInputComponent->BindAction(InterractAction, ETriggerEvent::Triggered, this, &ARewindCharacter::Interact);

		//Rewind
		if (ARewindGameMode* RewindGameMode = Cast<ARewindGameMode>(GetWorld()->GetAuthGameMode()))
		{
			EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Started, RewindGameMode, &ARewindGameMode::StartGlobalRewind);
			EnhancedInputComponent->BindAction(RewindAction, ETriggerEvent::Completed, RewindGameMode, &ARewindGameMode::EndGlobalRewind);
		}
		
		EnhancedInputComponent->BindAction(XRayAction, ETriggerEvent::Started, this, &ThisClass::StartXRay);
		EnhancedInputComponent->BindAction(XRayAction, ETriggerEvent::Completed, this, &ThisClass::StropXray);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARewindCharacter::DisableInputs()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
		}
	}
}

void ARewindCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(Catcher && Catch)
	{
		LookAtCatcher();
	}
}

void ARewindCharacter::Move(const FInputActionValue& Value)
{
	if (!CanMove){return;}
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARewindCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARewindCharacter::Jump()
{
	if (!CanMove){return;}
	Super::Jump();
}

void ARewindCharacter::Interact()
{
	OnPlayerInteracted.Broadcast();
}
