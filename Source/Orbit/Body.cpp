// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "Body.h"


// Sets default values
ABody::ABody()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	velocity = FVector::ZeroVector;

}

// Called when the game starts or when spawned
void ABody::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABody::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector newLocation = GetActorLocation();
	newLocation += velocity * DeltaTime;
	SetActorLocation(newLocation);
}

void ABody::SetVelocity(const FVector& v){

	velocity = v;
}

void ABody::AddVelocity(const FVector& v){

	velocity += v;
}

void ABody::ApplyForce(const FVector& f){

	AddVelocity(f / mass);
}

