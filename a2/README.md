# Assignment 2 | Meet Shah | 13d070003

I've added exhaustive comments in the code as and when needed in order 
to explain my logic behind the code. I'm repeating them here function wise.

### MeshVertex::updateQuadric()

- Obtain the `4*4` matrix corresponding to the face in context
- Assign it to quadric term 

### MeshEdge::updateQuadricCollapseError()

- Obtain the endpoints and their added quadrics
- Set the new_quadric with last row being a unit vector (0,0,0,1)
- Check for the invertibility of the obtained new quadric by checking the magnitude of the determinant
- If not invertible pass the midpoint of the endpoints as the collapse vertex.
- Else obtain the collapse position by multiplying the inverse with the unit vector. 
- Update the `collapse_error` accordingly 

### Mesh::decimateQuadricEdgeCollapse()
- I just followed the steps given in the `TODO`, nothing less or more.	
- Loop over edges to find the one with the minimum error (remember to check if the error is negative, in which case the edge is invalid for collapsing).
- Collapse the min error edge, if a valid one is found (else return NULL). There is a convenient function for this (it is a good exercise to study how this function operates). Make a note of the single vertex which is the result of the collapse.
- Update the normal of every face incident on this vertex.
- Update the quadric for this vertex, as well as the normal of the vertex (there are convenient functions for this, one of which you will write yourself).
- For every edge incident on this vertex:
	* Update the quadric and normal for its other endpoint, which has also been affected.
    * Update the quadric collapse error and the optimal collapse position for the edge.
- Return the vertex.

## Standard Library Functionality Usage

- None

## REFERENCES

- [Trimesh](https://github.com/mikedh/trimesh)- I perused the code to read `.off` files only to realize that we don't have to implement it.

## Additional Notes

- I've tested the algorithm implementation by running it iteration by iteration on torus and cude and it works fine. I'd wanted to add the simplified bunny `.off` files as comparison with the ones provided, however the size constraints prevents me from doing so. 

## Completion

- (a) Did not do EC1
- (b) Did not do EC2
- (c) Did everything required
