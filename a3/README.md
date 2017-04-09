# Assignment 3 | Meet Shah | 13d070003

I've added exhaustive comments in the code as and when needed in order 
to explain my logic behind the code. I'm repeating them here function wise.

## Results

======================================
Top K	avgPrecision	avgRecall
======================================
1		0.048511576626	0.004007012186
5		0.049834619625	0.016366272016
10		0.048621830209	0.031320732595
20		0.045589856670	0.060480371338
50		0.045953693495	0.145587366700
======================================

### main::computeVolume()

- Computes volume of the mesh using signedVolumes
- Computes volume of bbox using using axisaligned3bbox.volume() function
- Returns the ratio.

### main::signedVolumeOfTriangle()

- Computes the signed volume corresponding to a triangle face of the mesh.
- When these signed volumes are added together, the abs(sum) is equivalent to the volume of the mesh.   
- Please see the ICIP 2001 paper in reference [1] for the same.

### ComputeResults.py

#### readClaFile(filename)
	- Reads the test.cla file and allots mesh paths to a lookup table (dictionary). 

#### getShapeDescriptor()
	- Computes shapeDesc for all the meshes by calling the compiled binary and passing the path as argument
	- Saves the descriptors along with class info to a csv file.

#### computePrecisonRecall()
	- Computes the precision and recall for different values of k ( for top-k).

## Standard Library Functionality Usage

- None

## REFERENCES

- [Chen.et.al](http://chenlab.ece.cornell.edu/Publication/Cha/icip01_Cha.pdf) - The idea of signed volume computation and equivalence with volume is taken from this paper. 
- [GTS](http://gts.sourceforge.net/reference/gts-surfaces.html#GTS-SURFACE-VOLUME) - Did not see the code, but for processing reference


## Additional Notes

- Computation takes a lot of time for all 907 meshes, so I computed the descriptors once, saved them in one csvand then used it for precision and recall computations.

## Completion

- (b) Did not do EC2
- (c) Did everything required
