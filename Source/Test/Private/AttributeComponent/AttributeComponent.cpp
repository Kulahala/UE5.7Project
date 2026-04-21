// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Gold = 0;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::AddGold(int32 Amount)
{
	Gold += Amount;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
		                                 FString::Printf(TEXT("获得了 %d 金币！总金币:%d"), Amount, Gold));
	}
}
