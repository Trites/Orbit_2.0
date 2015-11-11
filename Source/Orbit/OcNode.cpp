// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "OcNode.h"

OcNode::OcNode(FVector origin, FVector size) 
: origin(origin), size(size), isLeaf(true), isUsed(false){

	for (int i = 0; i < CHILD_COUNT; ++i)
		children[i] = nullptr;

	centerMass.position = FVector::ZeroVector;
	centerMass.mass = 0.0f;
	storedBody = nullptr;
}

OcNode::~OcNode()
{
	for (int i = 0; i < CHILD_COUNT; ++i)
		delete children[i];
}

void OcNode::draw(UWorld *world){


	if (isUsed)
		DrawDebugPoint(world, centerMass.position, 16, FColor::Green);

	if (!isLeaf){

		for (OcNode* child : children)
			child->draw(world);


	}
	//else{

	//	FColor renderColor = storedBody != nullptr ? FColor::Red : FColor::Blue;

	//	FVector topLeftBack(origin.X - size.X/2, origin.Y + size.Y/2, origin.Z - size.Z/2);
	//	FVector topRightBack(origin.X + size.X / 2, origin.Y + size.Y / 2, origin.Z - size.Z / 2);
	//	FVector bottomLeftBack(origin.X - size.X / 2, origin.Y - size.Y / 2, origin.Z - size.Z / 2);
	//	FVector bottomRightBack(origin.X + size.X / 2, origin.Y - size.Y / 2, origin.Z - size.Z / 2);

	//	FVector topLeftFront(origin.X - size.X / 2, origin.Y + size.Y / 2, origin.Z + size.Z / 2);
	//	FVector topRightFront(origin.X + size.X / 2, origin.Y + size.Y / 2, origin.Z + size.Z / 2);
	//	FVector bottomLeftFront(origin.X - size.X / 2, origin.Y - size.Y / 2, origin.Z + size.Z / 2);
	//	FVector bottomRightFront(origin.X + size.X / 2, origin.Y - size.Y / 2, origin.Z + size.Z / 2);

	//	DrawDebugLine(world, topLeftBack, topRightBack, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, topRightBack, bottomRightBack, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, bottomRightBack, bottomLeftBack, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, bottomLeftBack, topLeftBack, renderColor, false, -1, 0, DEBUG_THIKNESS);

	//	DrawDebugLine(world, topLeftFront, topRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, topRightFront, bottomRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, bottomRightFront, bottomLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, bottomLeftFront, topLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);

	//	DrawDebugLine(world, topLeftBack, topLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, topRightBack, topRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, bottomRightBack, bottomRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//	DrawDebugLine(world, bottomLeftBack, bottomLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	//}
}

void OcNode::insert(ABody* body){

	//Each node stores the center mass for all its children.
	centerMass.adjust(body->GetActorLocation(), body->mass);

	if (isLeaf){

		//If node is empty we store the body and return
		if (storedBody == nullptr){

			storedBody = body;
			return;
		}
		else{

			split();

			//Move contained object appropriate child node.
			children[indexOf(storedBody->GetActorLocation())]->insert(storedBody);
			storedBody = nullptr;
		}
	}

	//Insert new body into the appropriate child node.
	children[indexOf(body->GetActorLocation())]->insert(body);
}

void OcNode::insert(std::vector<ABody*> bodies){

	for (ABody* body : bodies)
		insert(body);
}

bool OcNode::contains(ABody* body){

	return storedBody == body;
}

const FVector& OcNode::getSize() const{

	return size;
}

void OcNode::split(){

	isLeaf = false;

	for (std::size_t i = 0; i < CHILD_COUNT; i++){

		FVector childOrigin = origin;
		FVector quarterSize = size * 0.25f;

		childOrigin.X += i & 1 ? quarterSize.X : -quarterSize.X;
		childOrigin.Y += i & 2 ? quarterSize.Y : -quarterSize.Y;
		childOrigin.Z += i & 4 ? quarterSize.Z : -quarterSize.Z;

		children[i] = new OcNode(childOrigin, size * 0.5f);
	}
}

std::size_t OcNode::indexOf(const FVector& point) const{

	std::size_t index = 0;

	if (point.X >= origin.X) index += 1;
	if (point.Y >= origin.Y) index += 2;
	if (point.Z >= origin.Z) index += 4;
	return index;
}