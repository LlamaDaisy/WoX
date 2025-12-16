// Fill out your copyright notice in the Description page of Project Settings.


#include "FPCharacter.h"
#include "I_Interactable.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "InputAction.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AFPCharacter::AFPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
}

// Called when the game starts or when spawned
void AFPCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetActorRotation().Yaw;
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}

}

void AFPCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D DirectionValue = Value.Get<FVector2D>();

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDir,DirectionValue.Y);
	const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDir, DirectionValue.X);
}

void AFPCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(-LookAxisVector.X);

	FRotator ControlRot = Controller->GetControlRotation();

	ControlRot.Pitch = FMath::ClampAngle(ControlRot.Pitch, MinPitch, MaxPitch);

	float CurrentYaw = ControlRot.Yaw;
	float BodyYaw = GetActorRotation().Yaw;
	float DeltaYaw = FMath::FindDeltaAngleDegrees(BodyYaw, CurrentYaw);

	if (DeltaYaw < MinYaw)
	{
		ControlRot.Yaw = BodyYaw + MinYaw;
	}

	else if (DeltaYaw > MaxYaw)
	{
		ControlRot.Yaw = BodyYaw + MaxYaw;
	}

	Controller->SetControlRotation(ControlRot);
}

void AFPCharacter::Jump()
{
	Super::Jump();
}

void AFPCharacter::ToggleCrouch()
{
	if (isCrouching)
	{
		UnCrouch();
		isCrouching = false;
	}

	else 
	{
		Crouch();
		isCrouching = true;
	}

}

void AFPCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact Pressed"));

	if (!Controller) return;

	FVector Start;
	FRotator ViewRot;
	Controller->GetPlayerViewPoint(Start, ViewRot);

	if (InteractDistance <= 0.f)
	{
		InteractDistance = 250.f;
	}

	FVector End = Start + (ViewRot.Vector() * InteractDistance);

	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor::Green,
		false,
		1.0f,
		0,
		1.5f
		);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Hit"));
		return;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit, but no actor"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s (%s)"),
		*HitActor->GetName(),
		*HitActor->GetClass()->GetName());

	if (HitActor->GetClass()->ImplementsInterface(UI_Interactable::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor has I_Interactable, calling Interact"));
		II_Interactable::Execute_Interact(HitActor, this);
	}

	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor does not have I_Interactable"));
	}
}

// Called every frame
void AFPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	 
	if (!Controller) return;

	FRotator ControlRot = Controller->GetControlRotation();
	FRotator BodyRot = GetActorRotation();

	float DeltaYaw = FMath::FindDeltaAngleDegrees(BodyRot.Yaw, ControlRot.Yaw);

	if (FMath::Abs(DeltaYaw) > (MaxYaw * 0.8f))
	{
		FRotator TargetRot = FRotator(0.f, ControlRot.Yaw, 0.f);
		BodyRot = FMath::RInterpTo(BodyRot, TargetRot, DeltaTime, BodyFollowSpeed);
		SetActorRotation(BodyRot);
	}
}

// Called to bind functionality to input
void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPCharacter::Look);
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPCharacter::Jump);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AFPCharacter::ToggleCrouch);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFPCharacter::Interact);

	}

}

