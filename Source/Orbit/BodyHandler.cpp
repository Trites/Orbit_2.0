// Fill out your copyright notice in the Description page of Project Settings.
#define _USE_MATH_DEFINES
#include "Orbit.h"
#include "BodyHandler.h"
#include "OcNode.h"
#include <unordered_set>
#include <math.h>

// Sets default values
ABodyHandler::ABodyHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABodyHandler::BeginPlay()
{
	Super::BeginPlay();

	SpawnDisc(FVector::ZeroVector, 1000, 800);
	int c = 0;
	for (TActorIterator<ABody> it(GetWorld()); it; ++it){

		c++;
		//SetInitialVelocity(*it);
		Bodies.push_back(*it);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Simulating %i bodies."), Bodies.size()));

	//FActorSpawnParameters spawnParams;
	//spawnParams.Owner = this;

}

// Called every frame
void ABodyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector galaxyCenter = FVector::ZeroVector;
	float galaxySize = 0;

	GetGalaxyArea(galaxyCenter, galaxySize);

	delete ocNode;
	ocNode = new OcNode(galaxyCenter, FVector(galaxySize, galaxySize, galaxySize));
	ocNode->insert(Bodies);
	ocNode->draw(GetWorld());

	for (ABody* body : Bodies)
		CalculateForce(body, ocNode, DeltaTime);
}

void ABodyHandler::CalculateForce(ABody* body, OcNode* node, float DeltaTime){

	if (node->contains(body) == false){

		OcNode::CenterMass centerMass = node->centerMass;

		if (centerMass.mass > 0 && FVector::DistSquared(body->GetActorLocation(), centerMass.position) > 100){

			FVector direction = (centerMass.position - body->GetActorLocation()).GetSafeNormal();
			float force = (10.0f * body->mass * centerMass.mass) / FVector::DistSquared(centerMass.position, body->GetActorLocation());
			body->ApplyForce(DeltaTime * force * direction);
		}
	}
	else{

		if (!node->isLeaf)
			for (OcNode* child : node->children){

				CalculateForce(body, child, DeltaTime);
			}
	}
}

void ABodyHandler::SetInitialVelocity(ABody *body, float force){

	FVector directionToCenter = (body->GetActorLocation() - FVector::ZeroVector).GetSafeNormal();
	FVector forceDirection(directionToCenter.Y, -directionToCenter.X, directionToCenter.Z);

	body->ApplyForce(forceDirection * force);
}

void ABodyHandler::GetGalaxyArea(FVector& center, float& size, float padding){

	FVector min = FVector::ZeroVector;
	FVector max = FVector::ZeroVector;

	for (ABody* body : Bodies)
	{
		FVector pos = body->GetActorLocation();

		if (pos.X < min.X)
			min.X = pos.X;
		else if (pos.X > max.X)
			max.X = pos.X;

		if (pos.Y < min.Y)
			min.Y = pos.Y;
		else if (pos.Y > max.Y)
			max.Y = pos.Y;

		if (pos.Z < min.Z)
			min.Z = pos.Z;
		else if (pos.Z > max.Z)
			max.Z = pos.Z;
	}

	center = min + (max - min) * 0.5f;
	size = std::fmaxf(std::fmaxf(max.X - min.X, max.Y - min.Y), max.Z - min.Z) + padding;
}


void ABodyHandler::SpawnDisc(const FVector& center, float radius, int objectCount){

	UWorld* world = GetWorld();

	FVector position;

	for (int i = 0; i < objectCount; i++){

		float angle = (static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * M_PI * 2;
		float dist = std::sqrt(static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * radius;

		position = FVector(center.X + dist * std::cos(angle), center.Y + dist * std::sin(angle), center.Z);

		ABody *body = GetWorld()->SpawnActor<ABody>(BodyTemplate, position, FRotator::ZeroRotator);
		SetInitialVelocity(body, 100*dist);
	}
}
