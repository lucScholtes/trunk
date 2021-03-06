# -*- coding: utf-8 -*-
from yade import pack
from utils import *
############################################
###   DEFINING VARIABLES AND MATERIALS   ###
############################################

# The following 5 lines will be used later for batch execution
nRead=utils.readParamsFromTable(
	num_spheres=500,# number of spheres
	compFricDegree = 30, # contact friction during the confining phase
	unknownOk=True
)
from yade.params import table

num_spheres=table.num_spheres# number of spheres
compFricDegree = table.compFricDegree # contact friction during the confining phase
finalFricDegree = 30 # contact friction during the deviatoric loading
rate=0.02 # loading rate (strain rate)
damp=0.2 # damping coefficient
stabilityThreshold=0.01 # we test unbalancedForce against this value in different loops (see below)
key='_triax_base_' # put you simulation's name here
young=5e6 # contact stiffness
mn,mx=Vector3(0,0,0),Vector3(1,1,1) # corners of the initial packing
thick = 0.01 # thickness of the plates

## create materials for spheres and plates
O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=radians(compFricDegree),density=2600,label='spheres'))
O.materials.append(FrictMat(young=young,poisson=0.5,frictionAngle=0,density=0,label='walls'))

## create walls around the packing
walls=utils.aabbWalls([mn,mx],thickness=thick,material='walls')
wallIds=O.bodies.append(walls)

## use a SpherePack object to generate a random loose particles packing
sp=pack.SpherePack()
sp.makeCloud(mn,mx,-1,0.3333,num_spheres,False, 0.95)

## approximate mean rad of the futur dense packing for latter use (as an exercise: you can compute exact 
volume = (mx[0]-mn[0])*(mx[1]-mn[1])*(mx[2]-mn[2])
mean_rad = pow(0.09*volume/num_spheres,0.3333)


clumps=False
if clumps:
	c1=pack.SpherePack([((-0.2*mean_rad,0,0),0.5*mean_rad),((0.2*mean_rad,0,0),0.5*mean_rad)])
	sp.makeClumpCloud((0,0,0),(1,1,1),[c1],periodic=False)
	O.bodies.append([utils.sphere(center,rad,material='spheres') for center,rad in sp])
	standalone,clumps=sp.getClumps()
	for clump in clumps:
		O.bodies.clump(clump)
		for i in clump[1:]: O.bodies[i].shape.color=O.bodies[clump[0]].shape.color
	#sp.toSimulation()
else:
	O.bodies.append([utils.sphere(center,rad,material='spheres') for center,rad in sp])

O.dt=.5*utils.PWaveTimeStep() # initial timestep, to not explode right away
O.usesTimeStepper=True

############################
###   DEFINING ENGINES   ###
############################

triax=ThreeDTriaxialEngine(
	## ThreeDTriaxialEngine will be used to control stress and strain. It controls particles size and plates positions.
	maxMultiplier=1.+2e4/young, # spheres growing factor (fast growth)
	finalMaxMultiplier=1.+2e3/young, # spheres growing factor (slow growth)
	thickness = thick,
	stressControl_1 = False, #switch stress/strain control
	stressControl_2 = False,
	stressControl_3 = False,
	## The stress used for (isotropic) internal compaction
	sigma_iso = 10000,
	## Independant stress values for anisotropic loadings
	sigma1=10000,
	sigma2=10000,
	sigma3=10000,
	internalCompaction=True, # If true the confining pressure is generated by growing particles
	Key=key, # passed to the engine so that the output file will have the correct name
)

newton=NewtonIntegrator(damping=damp)

O.engines=[
	ForceResetter(),
	InsertionSortCollider([Bo1_Sphere_Aabb(),Bo1_Box_Aabb()]),
	InteractionLoop(
		[Ig2_Sphere_Sphere_ScGeom(),Ig2_Box_Sphere_ScGeom()],
		[Ip2_FrictMat_FrictMat_FrictPhys()],
		[Law2_ScGeom_FrictPhys_CundallStrack()]
	),
	GlobalStiffnessTimeStepper(active=1,timeStepUpdateInterval=100,timestepSafetyCoefficient=0.8, defaultDt=4*utils.PWaveTimeStep()),
	triax,
	TriaxialStateRecorder(iterPeriod=100,file='WallStresses'+key),
	newton
]

#Display spheres with 2 colors for seeing rotations better
Gl1_Sphere.stripes=0
if nRead==0: yade.qt.Controller(), yade.qt.View()

#######################################
###   APPLYING CONFINING PRESSURE   ###
#######################################

#while 1:
  #O.run(1000, True)
  ##the global unbalanced force on dynamic bodies, thus excluding boundaries, which are not at equilibrium
  #unb=unbalancedForce()
  ##average stress
  ##note: triax.stress(k) returns a stress vector, so we need to keep only the normal component
  #meanS=(triax.stress(triax.wall_right_id)[0]+triax.stress(triax.wall_top_id)[1]+triax.stress(triax.wall_front_id)[2])/3
  #print 'unbalanced force:',unb,' mean stress: ',meanS
  #if unb<stabilityThreshold and abs(meanS-triax.sigma_iso)/triax.sigma_iso<0.001:
    #break

#O.save('confinedState'+key+'.yade.gz')
#print "###      Isotropic state saved      ###"

##############################
###   DEVIATORIC LOADING   ###
##############################

##We move to deviatoric loading, let us turn internal compaction off to keep particles sizes constant
#triax.internalCompaction=False

## Change contact friction (remember that decreasing it would generate instantaneous instabilities)
#triax.setContactProperties(finalFricDegree)

##set independant stress control on each axis
#triax.stressControl_1=triax.stressControl_2=triax.stressControl_3=True
## We turn all these flags true, else boundaries will be fixed
#triax.wall_bottom_activated=True
#triax.wall_top_activated=True
#triax.wall_left_activated=True
#triax.wall_right_activated=True
#triax.wall_back_activated=True
#triax.wall_front_activated=True


##If we want a triaxial loading at imposed strain rate, let's assign srain rate instead of stress
#triax.stressControl_2=0 #we are tired of typing "True" and "False", we use implicit conversion from integer to boolean
#triax.strainRate2=rate
#triax.strainRate1=100*rate
#triax.strainRate3=100*rate

##we can change damping here. What is the effect in your opinion?
#newton.damping=0.1

##Save temporary state in live memory. This state will be reloaded from the interface with the "reload" button.
#O.saveTmp()

#####################################################
###    Exemple of how to record and plot data     ###
#####################################################

#from yade import plot

### a function saving variables
#def history():
  	#plot.addData(e11=triax.strain[0], e22=triax.strain[1], e33=triax.strain[2],
		    #s11=triax.stress(triax.wall_right_id)[0],
		    #s22=triax.stress(triax.wall_top_id)[1],
		    #s33=triax.stress(triax.wall_front_id)[2],
		    #i=O.iter)

#if 1:
  ## include a periodic engine calling that function in the simulation loop
  #O.engines=O.engines[0:5]+[PyRunner(iterPeriod=20,command='history()',label='recorder')]+O.engines[5:7]
  ##O.engines.insert(4,PyRunner(iterPeriod=20,command='history()',label='recorder'))
#else:
  ## With the line above, we are recording some variables twice. We could in fact replace the previous
  ## TriaxialRecorder
  ## by our periodic engine. Uncomment the following line:
  #O.engines[4]=PyRunner(iterPeriod=20,command='history()',label='recorder')

#O.run(100,True)

### declare what is to plot. "None" is for separating y and y2 axis
#plot.plots={'i':('e11','e22','e33',None,'s11','s22','s33')}

##display on the screen (doesn't work on VMware image it seems)
##plot.plot()

## In that case we can still save the data to a text file at the the end of the simulation, with: 
#plot.saveDataTxt('results'+key)
##or even generate a script for gnuplot. Open another terminal and type  "gnuplot plotScriptKEY.gnuplot:
#plot.saveGnuplot('plotScript'+key)
