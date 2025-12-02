// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FPCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class WOX_API AFPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void ToggleCrouch();
	virtual void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinYaw = -90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxYaw = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinPitch = -60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxPitch = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BodyFollowSpeed = 5.f;

	float InitialYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool isCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float InteractDistance = 250.f;



	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

	
};
