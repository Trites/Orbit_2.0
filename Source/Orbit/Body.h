// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Body.generated.h"

UCLASS()
class ORBIT_API ABody : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABody();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetVelocity(const FVector& v);
	void AddVelocity(const FVector& v);
	void ApplyForce(const FVector& f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body")
	float mass = 300.0f;

private:

	float runningTime;
	FVector velocity;

	UStaticMeshComponent* mesh;
};
