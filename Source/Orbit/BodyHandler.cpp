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

	int m = 0;
	//Find bodies that are already active in the scene.

	//Spawn new bodies
	SpawnDisc(FVector::ZeroVector, 2000, 5000, 100.0f);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Simulating %i bodies."), Bodies.size()));
}

void ABodyHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector galaxyCenter = FVector::ZeroVector;
	float galaxySize = 0;

	GetGalaxyArea(galaxyCenter, galaxySize);


	ocNode = new OcNode(galaxyCenter, FVector(galaxySize, galaxySize, galaxySize));
	ocNode->insert(Bodies);

	for (ABody* body : Bodies)
		SimulateBody(body, ocNode, DeltaTime);

	ocNode->draw(GetWorld());

	delete ocNode;
}

void ABodyHandler::SimulateBody(ABody* body, OcNode* node, float DeltaTime){

	OcNode::CenterMass centerMass = node->centerMass;

	//If node hass no mass it can be disregarded
	if (centerMass.mass > 0){

		//Keep distance as squared to avoid expensive sqrt()
		float distSquared = FVector::DistSquared(body->GetActorLocation(), centerMass.position);
 
		//Calculate theta, assuming node is of uniform size
		float thetaSquared = (node->getSize().X * node->getSize().X) / distSquared;

		//If theta is acceptable or if the node only contains one body we use for calculation
		//Otherwise we traverse further down the tree
		if (thetaSquared < MAX_THETA_SQUARED || node->isLeaf){

			//Make sure that we don't calculate force from node that only contains current body
			if (node->contains(body) == false){
				FVector direction = (centerMass.position - body->GetActorLocation()).GetSafeNormal();
				float force = (10.0f * body->mass * centerMass.mass) / distSquared;
				body->ApplyForce(DeltaTime * force * direction);
				node->isUsed = true;
			}
		}
		else{
			//If current node has children
			if (!node->isLeaf){
				for (OcNode* child : node->children){
					SimulateBody(body, child, DeltaTime);
				}
			}
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


void ABodyHandler::SpawnDisc(const FVector& center, float radius, int objectCount, float zJitter){

	UWorld* world = GetWorld();

	FVector position;

	int armCount = 8;
	int bodyPerArm = objectCount / armCount;
	float angleStep = (M_PI * 2) / armCount;
	float angleSpread = M_PI / 4;

	for (int i = 0; i < objectCount; i++){




		//float angle = (static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * M_PI * 2;
		float dist = std::sqrt(static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * radius;
		float angleJitter = (static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * angleSpread / (dist*dist*dist*0.0000000001) - (angleSpread / (dist*dist*dist*0.0000000001)) / 2;
		float angle = (i / bodyPerArm) * angleStep + angleJitter;

		//float dist = (static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)) * radius;

		float zOffset = 100 * (zJitter * static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX)-zJitter / 2) / (std::sqrt(dist) + 1);

		position = FVector(center.X + dist * std::cos(angle), center.Y + dist * std::sin(angle), center.Z + zOffset);

		ABody *body = GetWorld()->SpawnActor<ABody>(BodyTemplate, position, FRotator::ZeroRotator);

		float mass = 15 / dist + 100;

		SetInitialVelocity(body, 400 * std::sqrt(dist) );
		Bodies.push_back(body);
	}
}
