// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"


#include "GameFramework/SpringArmComponent.h"
// Sets default values
ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BirdCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BirdCapsule"));

	SetRootComponent(BirdCapsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(BirdCapsule);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprinArm"));
	SpringArm->SetupAttachment(BirdCapsule);
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);


	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value)
{
	if (Controller && Value != 0)
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);

	}
}

void ABird::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABird::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Moveforward"), this, &ABird::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABird::Turn);
	PlayerInputComponent->BindAxis(TEXT("Lookup"), this, &ABird::LookUp);
}

