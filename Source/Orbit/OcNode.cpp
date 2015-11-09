// Fill out your copyright notice in the Description page of Project Settings.

#include "Orbit.h"
#include "OcNode.h"

OcNode::OcNode(FVector origin, FVector size) 
: origin(origin), size(size), isLeaf(true){

	for (int i = 0; i < CHILD_COUNT; ++i)
		children[i] = nullptr;

	centerMass.position = FVector::ZeroVector;
	centerMass.mass = 0.0f;
	objectCount = 0;
	storedBody = nullptr;
}

OcNode::~OcNode()
{
	for (int i = 0; i < CHILD_COUNT; ++i)
		delete children[i];
}

void OcNode::draw(UWorld *world){

	if (!isLeaf){

		for (OcNode* child : children)
			child->draw(world);


		if (objectCount > 0)
			DrawDebugPoint(world, centerMass.position, 16, FColor::Green);
	}
	else{

		FColor renderColor = objectCount > 0 ? FColor::Red : FColor::Blue;

		FVector topLeftBack(origin.X - size.X/2, origin.Y + size.Y/2, origin.Z - size.Z/2);
		FVector topRightBack(origin.X + size.X / 2, origin.Y + size.Y / 2, origin.Z - size.Z / 2);
		FVector bottomLeftBack(origin.X - size.X / 2, origin.Y - size.Y / 2, origin.Z - size.Z / 2);
		FVector bottomRightBack(origin.X + size.X / 2, origin.Y - size.Y / 2, origin.Z - size.Z / 2);

		FVector topLeftFront(origin.X - size.X / 2, origin.Y + size.Y / 2, origin.Z + size.Z / 2);
		FVector topRightFront(origin.X + size.X / 2, origin.Y + size.Y / 2, origin.Z + size.Z / 2);
		FVector bottomLeftFront(origin.X - size.X / 2, origin.Y - size.Y / 2, origin.Z + size.Z / 2);
		FVector bottomRightFront(origin.X + size.X / 2, origin.Y - size.Y / 2, origin.Z + size.Z / 2);

		DrawDebugLine(world, topLeftBack, topRightBack, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, topRightBack, bottomRightBack, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, bottomRightBack, bottomLeftBack, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, bottomLeftBack, topLeftBack, renderColor, false, -1, 0, DEBUG_THIKNESS);

		DrawDebugLine(world, topLeftFront, topRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, topRightFront, bottomRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, bottomRightFront, bottomLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, bottomLeftFront, topLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);

		DrawDebugLine(world, topLeftBack, topLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, topRightBack, topRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, bottomRightBack, bottomRightFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
		DrawDebugLine(world, bottomLeftBack, bottomLeftFront, renderColor, false, -1, 0, DEBUG_THIKNESS);
	}
}

bool OcNode::insert(ABody* body){

	centerMass.adjust(body->GetActorLocation(), body->mass);
	objectCount++;

	//objects.insert(body);


	if (isLeaf){

		//If there is still room in this node we insert the new body.
		//Otherwise we split the node.
		if (storedBody == nullptr){

			storedBody = body;
		}
		else{

			split();
			////Insert new body into the appropriate child node.
			children[indexOf(body->GetActorLocation())]->insert(body);

			children[indexOf(storedBody->GetActorLocation())]->insert(storedBody);
			storedBody = nullptr;

			////Move all objects contained in this node to the appropriate child node.
			//for (ABody* oldBody : objects){

			//	children[indexOf(oldBody->GetActorLocation())]->insert(oldBody);
			//}
			//objects.clear();
		}
	}
	else{

		//This node is not a leaf, so we insert the new body into the appropriate child.
		children[indexOf(body->GetActorLocation())]->insert(body);
	}

	return false;
}

void OcNode::insert(std::vector<ABody*> bodies){

	for (ABody* body : bodies)
		insert(body);
}

bool OcNode::contains(ABody* body){

	if (storedBody == nullptr)
		return false;

	return storedBody == body;
	//return objects.count(body) != 0;
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