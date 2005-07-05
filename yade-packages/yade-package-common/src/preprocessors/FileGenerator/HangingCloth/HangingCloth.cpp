/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "HangingCloth.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Body.hpp>
#include <yade/yade-common/Mesh2D.hpp>
#include <yade/yade-common/Sphere.hpp>
#include <yade/yade-common/RigidBodyParameters.hpp>
#include <yade/yade-common/Box.hpp>
#include <yade/yade-common/AABB.hpp>
#include <yade/yade-common/InteractingBox.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-common/FrictionLessElasticContactLaw.hpp>
#include <yade/yade-common/PersistentSAPCollider.hpp>
#include <yade/yade-common/MassSpringLaw.hpp>
#include <yade/yade-common/MassSpringBody2RigidBodyLaw.hpp>

#include <yade/yade-lib-serialization/IOManager.hpp>
#include <yade/yade-common/InteractionGeometryMetaEngine.hpp>
#include <yade/yade-common/InteractionPhysicsMetaEngine.hpp>

#include <yade/yade-common/PhysicalActionApplier.hpp>
#include <yade/yade-common/PhysicalActionDamper.hpp>

#include <yade/yade-common/CundallNonViscousForceDamping.hpp>
#include <yade/yade-common/CundallNonViscousMomentumDamping.hpp>

#include <yade/yade-common/BoundingVolumeMetaEngine.hpp>
#include <yade/yade-common/GeometricalModelMetaEngine.hpp>

#include <yade/yade-common/InteractionDescriptionSet2AABB.hpp>
#include <yade/yade-common/MetaInteractingGeometry.hpp>
#include <yade/yade-common/ParticleParameters.hpp>
#include <yade/yade-common/ParticleSetParameters.hpp>
#include <yade/yade-common/SpringGeometry.hpp>
#include <yade/yade-common/SpringPhysics.hpp>
#include <yade/yade-common/InteractingSphere.hpp>

#include <yade/yade-common/ElasticContactLaw.hpp>
#include <yade/yade-common/MacroMicroElasticRelationships.hpp>
#include <yade/yade-common/BodyMacroParameters.hpp>
#include <yade/yade-common/SDECLinkGeometry.hpp>
#include <yade/yade-common/SDECLinkPhysics.hpp>
#include <yade/yade-common/MassSpringBody2RigidBodyLaw.hpp>

#include <yade/yade-common/PhysicalActionContainerReseter.hpp>
#include <yade/yade-common/PhysicalActionContainerInitializer.hpp>
#include <yade/yade-common/GravityEngine.hpp>

#include <yade/yade-common/PhysicalParametersMetaEngine.hpp>

#include <yade/yade-common/BodyRedirectionVector.hpp>
#include <yade/yade-common/InteractionVecSet.hpp>
#include <yade/yade-common/PhysicalActionVectorVector.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

HangingCloth::HangingCloth () : FileGenerator()
{
	width = 20;
	height = 20;
	springStiffness = 400000;//500;
	springDamping   = 1.0;
	particleDamping = 0.997;
	clothMass = 4000;//10;
	cellSize = 20;
	fixPoint1 = true;
	fixPoint2 = true;
	fixPoint3 = true;
	fixPoint4 = true;
	ground = true;
	dampingForce = 0.3;

// spheres	
	nbSpheres = Vector3r(2,3,4);
	minRadius = 6;
	maxRadius = 10;
	disorder = 2.1;
	spacing = 17;
	dampingMomentum = 0.3;
	density = 2;
	linkSpheres = true;

	sphereYoungModulus   = 10000;
	spherePoissonRatio  = 0.2;
	sphereFrictionDeg   = 18.0;
	gravity 	= Vector3r(0,-9.81,0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

HangingCloth::~HangingCloth ()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void HangingCloth::postProcessAttributes(bool)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void HangingCloth::registerAttributes()
{
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(height);
	REGISTER_ATTRIBUTE(springStiffness);
	REGISTER_ATTRIBUTE(springDamping);
	//REGISTER_ATTRIBUTE(particleDamping); 	// FIXME - ignored - delete it, or start using it
	REGISTER_ATTRIBUTE(clothMass);
	REGISTER_ATTRIBUTE(cellSize);
	REGISTER_ATTRIBUTE(fixPoint1);
	REGISTER_ATTRIBUTE(fixPoint2);
	REGISTER_ATTRIBUTE(fixPoint3);
	REGISTER_ATTRIBUTE(fixPoint4);
	REGISTER_ATTRIBUTE(ground);
	
	// spheres
	//REGISTER_ATTRIBUTE(nbSpheres);
	//REGISTER_ATTRIBUTE(minRadius);
	//REGISTER_ATTRIBUTE(maxRadius);
	//rEGISTER_ATTRIBUTE(density); 		
	//REGISTER_ATTRIBUTE(disorder);
	//REGISTER_ATTRIBUTE(spacing);		
	
	REGISTER_ATTRIBUTE(gravity);
	REGISTER_ATTRIBUTE(dampingForce);
	REGISTER_ATTRIBUTE(dampingMomentum);
	//REGISTER_ATTRIBUTE(linkSpheres);

//	REGISTER_ATTRIBUTE(sphereYoungModulus);
//	REGISTER_ATTRIBUTE(spherePoissonRatio);
//	REGISTER_ATTRIBUTE(sphereFrictionDeg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

string HangingCloth::generate()
{
	rootBody = shared_ptr<MetaBody>(new MetaBody);

	rootBody->persistentInteractions	= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->volatileInteractions		= shared_ptr<InteractionContainer>(new InteractionVecSet);
	rootBody->actionParameters		= shared_ptr<PhysicalActionContainer>(new PhysicalActionVectorVector);
	rootBody->bodies 			= shared_ptr<BodyContainer>(new BodyRedirectionVector);

	shared_ptr<PhysicalActionContainerInitializer> actionParameterInitializer(new PhysicalActionContainerInitializer);
	actionParameterInitializer->actionParameterNames.push_back("Force");
	actionParameterInitializer->actionParameterNames.push_back("Momentum");
	
	shared_ptr<InteractionGeometryMetaEngine> interactionGeometryDispatcher(new InteractionGeometryMetaEngine);
	interactionGeometryDispatcher->add("InteractingSphere","InteractingSphere","Sphere2Sphere4MacroMicroContactGeometry");
	interactionGeometryDispatcher->add("InteractingSphere","InteractingBox","Box2Sphere4MacroMicroContactGeometry");

	shared_ptr<InteractionPhysicsMetaEngine> interactionPhysicsDispatcher(new InteractionPhysicsMetaEngine);
	interactionPhysicsDispatcher->add("BodyMacroParameters","BodyMacroParameters","MacroMicroElasticRelationships");

	shared_ptr<GravityEngine> gravityCondition(new GravityEngine);
	gravityCondition->gravity = gravity;
	
	shared_ptr<BoundingVolumeMetaEngine> boundingVolumeDispatcher	= shared_ptr<BoundingVolumeMetaEngine>(new BoundingVolumeMetaEngine);
	boundingVolumeDispatcher->add("InteractingSphere","AABB","Sphere2AABB");
	boundingVolumeDispatcher->add("InteractingBox","AABB","Box2AABB");
	boundingVolumeDispatcher->add("MetaInteractingGeometry","AABB","InteractionDescriptionSet2AABB");

	shared_ptr<GeometricalModelMetaEngine> geometricalModelDispatcher	= shared_ptr<GeometricalModelMetaEngine>(new GeometricalModelMetaEngine);
	geometricalModelDispatcher->add("ParticleSetParameters","Mesh2D","ParticleSet2Mesh2D");

	shared_ptr<CundallNonViscousForceDamping> actionForceDamping(new CundallNonViscousForceDamping);
	actionForceDamping->damping = dampingForce;
	shared_ptr<CundallNonViscousMomentumDamping> actionMomentumDamping(new CundallNonViscousMomentumDamping);
	actionMomentumDamping->damping = dampingMomentum;
	shared_ptr<PhysicalActionDamper> actionDampingDispatcher(new PhysicalActionDamper);
	actionDampingDispatcher->add("Force","ParticleParameters","CundallNonViscousForceDamping",actionForceDamping);
	actionDampingDispatcher->add("Momentum","RigidBodyParameters","CundallNonViscousMomentumDamping",actionMomentumDamping);
	
	shared_ptr<PhysicalActionApplier> applyActionDispatcher(new PhysicalActionApplier);
	applyActionDispatcher->add("Force","ParticleParameters","NewtonsForceLaw");
	applyActionDispatcher->add("Momentum","RigidBodyParameters","NewtonsMomentumLaw");

	shared_ptr<PhysicalParametersMetaEngine> positionIntegrator(new PhysicalParametersMetaEngine);
	positionIntegrator->add("ParticleParameters","LeapFrogPositionIntegrator");
	shared_ptr<PhysicalParametersMetaEngine> orientationIntegrator(new PhysicalParametersMetaEngine);
	orientationIntegrator->add("RigidBodyParameters","LeapFrogOrientationIntegrator");
 	
	shared_ptr<MassSpringLaw> explicitMassSpringConstitutiveLaw(new MassSpringLaw);
	explicitMassSpringConstitutiveLaw->springGroupMask = 1;

	shared_ptr<ElasticContactLaw> elasticContactLaw(new ElasticContactLaw);
	elasticContactLaw->sdecGroupMask = 2;

	//shared_ptr<MassSpringBody2RigidBodyLaw> massSpringBody2RigidBodyConstitutiveLaw(new MassSpringBody2RigidBodyLaw);
	//massSpringBody2RigidBodyConstitutiveLaw->sdecGroupMask = 2;
	//massSpringBody2RigidBodyConstitutiveLaw->springGroupMask = 1;

	rootBody->actors.clear();
	rootBody->actors.push_back(shared_ptr<Engine>(new PhysicalActionContainerReseter));
	rootBody->actors.push_back(boundingVolumeDispatcher);
	rootBody->actors.push_back(geometricalModelDispatcher);
	rootBody->actors.push_back(shared_ptr<Engine>(new PersistentSAPCollider));
	rootBody->actors.push_back(interactionGeometryDispatcher);
	rootBody->actors.push_back(interactionPhysicsDispatcher);
	rootBody->actors.push_back(explicitMassSpringConstitutiveLaw);
	rootBody->actors.push_back(elasticContactLaw);
//	rootBody->actors.push_back(massSpringBody2RigidBodyConstitutiveLaw); // FIXME - is not working...
	rootBody->actors.push_back(gravityCondition);
	rootBody->actors.push_back(actionDampingDispatcher);
	rootBody->actors.push_back(applyActionDispatcher);
	rootBody->actors.push_back(positionIntegrator);
	rootBody->actors.push_back(orientationIntegrator);

	rootBody->initializers.clear();
	rootBody->initializers.push_back(actionParameterInitializer);
	rootBody->initializers.push_back(boundingVolumeDispatcher);
	rootBody->initializers.push_back(geometricalModelDispatcher);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	// use ParticleSet so that we have automatic updating for free
	shared_ptr<ParticleSetParameters> physics2(new ParticleSetParameters);
	physics2->se3		= Se3r(Vector3r(0,0,0),q);

	rootBody->isDynamic	= false;

	shared_ptr<MetaInteractingGeometry> set(new MetaInteractingGeometry());
	set->diffuseColor	= Vector3f(0,0,1);

	shared_ptr<AABB> aabb(new AABB);
	aabb->diffuseColor	= Vector3r(0,0,1);

	shared_ptr<Mesh2D> mesh2d(new Mesh2D);
	mesh2d->diffuseColor	= Vector3f(0,0,1);
	mesh2d->wire		= false;
	mesh2d->visible		= true;
	mesh2d->shadowCaster	= false;

	rootBody->geometricalModel			= mesh2d;
	rootBody->interactionGeometry			= set;
	rootBody->boundingVolume			= aabb;
	rootBody->physicalParameters	= physics2;

	rootBody->persistentInteractions->clear();

//////////////////////////////////////////////////////
// make mesh
//////////////////////////////////////////////////////
	
	for(int i=0;i<width;i++)
		for(int j=0;j<height;j++)
		{
			shared_ptr<Body> node(new Body(0,1|2));

			node->isDynamic		= true;
			
			shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
			iSphere->diffuseColor	= Vector3f(0,0,1);
			iSphere->radius		= cellSize/2.2;

// BEGIN ORIGINAL							
//			shared_ptr<ParticleParameters> particle(new ParticleParameters);
//			particle->velocity		= Vector3r(0,0,0);
//			particle->mass			= clothMass/(Real)(width*height);
//			particle->se3			= Se3r(Vector3r(i*cellSize-(cellSize*(width-1))*0.5,0,j*cellSize-(cellSize*(height-1))*0.5),q);
// END									

// BEGIN HACK - to have collision between SDEC spheres and cloth	
			shared_ptr<BodyMacroParameters> particle(new BodyMacroParameters);
			particle->angularVelocity	= Vector3r(0,0,0);
			particle->velocity		= Vector3r(0,0,0);
			particle->mass			= clothMass/(Real)(width*height);
			particle->inertia		= Vector3r(2.0/5.0*particle->mass*iSphere->radius*iSphere->radius,2.0/5.0*particle->mass*iSphere->radius*iSphere->radius,2.0/5.0*particle->mass*iSphere->radius*iSphere->radius);
			particle->se3			= Se3r(Vector3r(i*cellSize-(cellSize*(width-1))*0.5,0,j*cellSize-(cellSize*(height-1))*0.5),q);
			particle->young			= sphereYoungModulus;
			particle->poisson		= spherePoissonRatio;
			particle->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;
// END HACK								

			shared_ptr<AABB> aabb(new AABB);
			aabb->diffuseColor	= Vector3r(0,1,0);

			node->boundingVolume		= aabb;
			//node->geometricalModel		= ??;
			node->interactionGeometry		= iSphere;
			node->physicalParameters= particle;

			rootBody->bodies->insert(node);
			mesh2d->vertices.push_back(particle->se3.position);
		}

	for(int i=0;i<width-1;i++)
		for(int j=0;j<height-1;j++)
		{
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i+1,j)));
			mesh2d->edges.push_back(Edge(offset(i,j),offset(i,j+1)));
			mesh2d->edges.push_back(Edge(offset(i,j+1),offset(i+1,j)));

			rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,j),offset(i+1,j)));
			rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,j),offset(i,j+1)));
			rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,j+1),offset(i+1,j)));

			vector<int> face1,face2;
			face1.push_back(offset(i,j));
			face1.push_back(offset(i+1,j));
			face1.push_back(offset(i,j+1));

			face2.push_back(offset(i+1,j));
			face2.push_back(offset(i+1,j+1));
			face2.push_back(offset(i,j+1));

			mesh2d->faces.push_back(face1);
			mesh2d->faces.push_back(face2);
		}

	for(int i=0;i<width-1;i++)
	{
		mesh2d->edges.push_back(Edge(offset(i,height-1),offset(i+1,height-1)));
		rootBody->persistentInteractions->insert(createSpring(rootBody,offset(i,height-1),offset(i+1,height-1)));

	}

	for(int j=0;j<height-1;j++)
	{
		mesh2d->edges.push_back(Edge(offset(width-1,j),offset(width-1,j+1)));
		rootBody->persistentInteractions->insert(createSpring(rootBody,offset(width-1,j),offset(width-1,j+1)));
	}

	if (fixPoint1)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(0,0)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3f(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	if (fixPoint2)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(width-1,0)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3f(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	if (fixPoint3)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(0,height-1)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3f(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	if (fixPoint4)
	{
		Body * body = static_cast<Body*>((*(rootBody->bodies))[offset(width-1,height-1)].get());
		ParticleParameters * p = static_cast<ParticleParameters*>(body->physicalParameters.get());
		p->invMass = 0;
		body->interactionGeometry->diffuseColor = Vector3f(1.0,0.0,0.0);
		body->isDynamic = false;
	}

	
//////////////////////////////////////////////////////
// make ground
//////////////////////////////////////////////////////

	shared_ptr<Body> groundBox;
	createBox(groundBox, Vector3r(0,-200,0), Vector3r(500,10,500));
	if(ground)
		rootBody->bodies->insert(groundBox);

//////////////////////////////////////////////////////
// make spheres
//////////////////////////////////////////////////////
	
// 	for(int i=0;i<nbSpheres[0];i++)
// 		for(int j=0;j<nbSpheres[1];j++)
// 			for(int k=0;k<nbSpheres[2];k++)
// 			{
// 				shared_ptr<Body> sphere;
// 				createSphere(sphere,i,j,k);
// 				rootBody->bodies->insert(sphere);
// 			}

			

//////////////////////////////////////////////////////
// make links
//////////////////////////////////////////////////////

	int linksNum=0;
	if(linkSpheres)
	{
		shared_ptr<Body> bodyA;
			
		for( rootBody->bodies->gotoFirst() ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
		{
			bodyA = rootBody->bodies->getCurrent();
			
			rootBody->bodies->pushIterator();
	
			rootBody->bodies->gotoNext();
			for( ; rootBody->bodies->notAtEnd() ; rootBody->bodies->gotoNext() )
			{
				shared_ptr<Body> bodyB;
				bodyB = rootBody->bodies->getCurrent();
	
				shared_ptr<BodyMacroParameters> a = dynamic_pointer_cast<BodyMacroParameters>(bodyA->physicalParameters);
				shared_ptr<BodyMacroParameters> b = dynamic_pointer_cast<BodyMacroParameters>(bodyB->physicalParameters);
				shared_ptr<InteractingSphere>	as = dynamic_pointer_cast<InteractingSphere>(bodyA->interactionGeometry);
				shared_ptr<InteractingSphere>	bs = dynamic_pointer_cast<InteractingSphere>(bodyB->interactionGeometry);
	
				if ( a && b && as && bs && (a->se3.position - b->se3.position).length() < (as->radius + bs->radius))  
				{
					shared_ptr<Interaction> 		link(new Interaction( bodyA->getId() , bodyB->getId() ));
					shared_ptr<SDECLinkGeometry>		geometry(new SDECLinkGeometry);
					shared_ptr<SDECLinkPhysics>	physics(new SDECLinkPhysics);
					
					geometry->radius1			= as->radius - fabs(as->radius - bs->radius)*0.5;
					geometry->radius2			= bs->radius - fabs(as->radius - bs->radius)*0.5;
	
					physics->initialKn			= 500000;
					physics->initialKs			= 50000;
					physics->heta				= 1;
					physics->initialEquilibriumDistance	= (a->se3.position - b->se3.position).length();
					physics->knMax				= 75000;
					physics->ksMax				= 7500;
	
					link->interactionGeometry 		= geometry;
					link->interactionPhysics 		= physics;
					link->isReal 				= true;
					link->isNew 				= false;
					
					rootBody->persistentInteractions->insert(link);
					++linksNum;
				}
			}
	
			rootBody->bodies->popIterator();
		}
	}
			
	return "total number of permament links created: " + lexical_cast<string>(linksNum);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<Interaction>& HangingCloth::createSpring(const shared_ptr<MetaBody>& rootBody,int i,int j)
{
	Body * b1 = static_cast<Body*>((*(rootBody->bodies))[i].get());
	Body * b2 = static_cast<Body*>((*(rootBody->bodies))[j].get());

	spring = shared_ptr<Interaction>(new Interaction( b1->getId() , b2->getId() ));
	shared_ptr<SpringGeometry>	geometry(new SpringGeometry);
	shared_ptr<SpringPhysics>	physics(new SpringPhysics);

	geometry->p1			= b1->physicalParameters->se3.position;
	geometry->p2			= b2->physicalParameters->se3.position;

	physics->initialLength		= (geometry->p1-geometry->p2).length();
	physics->stiffness		= springStiffness;
	physics->damping		= springDamping;

	spring->interactionGeometry = geometry;
	spring->interactionPhysics = physics;
	spring->isReal = true;
	spring->isNew = false;

	return spring;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void HangingCloth::createSphere(shared_ptr<Body>& body, int i, int j, int k)
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Sphere> gSphere(new Sphere);
	shared_ptr<InteractingSphere> iSphere(new InteractingSphere);
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);
	
		
	Vector3r position 		=   Vector3r(i,j,k) * spacing 
					  - Vector3r( nbSpheres[0]/2*spacing  ,  nbSpheres[1]/2*spacing-90  ,  nbSpheres[2]/2*spacing)
					  + Vector3r( Mathr::symmetricRandom()*disorder ,  Mathr::symmetricRandom()*disorder , Mathr::symmetricRandom()*disorder );

	Real radius 			= (Mathr::intervalRandom(minRadius,maxRadius));
	
	body->isDynamic			= true;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 4.0/3.0*Mathr::PI*radius*radius*radius*density; // FIXME!!!!!!!!!!!!!! I had a mistake everywhre - it was 4/3*PI*radius*radius, EVERYWHERE!!!. the volume of a sphere should be calculated in ONLY one place!!!
	physics->inertia		= Vector3r(2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius,2.0/5.0*physics->mass*radius*radius);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(0,1,0);


	gSphere->radius			= radius;
	gSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());
	gSphere->wire			= false;
	gSphere->visible		= true;
	gSphere->shadowCaster		= true;
	
	iSphere->radius			= radius;
	iSphere->diffuseColor		= Vector3f(Mathf::unitRandom(),Mathf::unitRandom(),Mathf::unitRandom());

	body->interactionGeometry	= iSphere;
	body->geometricalModel		= gSphere;
	body->boundingVolume		= aabb;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void HangingCloth::createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents)
{
	body = shared_ptr<Body>(new Body(0,2));
	shared_ptr<BodyMacroParameters> physics(new BodyMacroParameters);
	shared_ptr<AABB> aabb(new AABB);
	shared_ptr<Box> gBox(new Box);
	shared_ptr<InteractingBox> iBox(new InteractingBox);
	
	
	Quaternionr q;
	q.fromAxisAngle( Vector3r(0,0,1),0);

	body->isDynamic			= false;
	
	physics->angularVelocity	= Vector3r(0,0,0);
	physics->velocity		= Vector3r(0,0,0);
	physics->mass			= 0;
	physics->inertia		= Vector3r(0,0,0);
	physics->se3			= Se3r(position,q);
	physics->young			= sphereYoungModulus;
	physics->poisson		= spherePoissonRatio;
	physics->frictionAngle		= sphereFrictionDeg * Mathr::PI/180.0;

	aabb->diffuseColor		= Vector3r(1,0,0);

	gBox->extents			= extents;
	gBox->diffuseColor		= Vector3f(1,1,1);
	gBox->wire			= false;
	gBox->visible			= true;
	gBox->shadowCaster		= true;
	
	iBox->extents			= extents;
	iBox->diffuseColor		= Vector3f(1,1,1);

	body->boundingVolume		= aabb;
	body->interactionGeometry	= iBox;
	body->geometricalModel		= gBox;
	body->physicalParameters	= physics;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////