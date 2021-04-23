# Clustering
## Overview
This is the command-line program which can do many fancy things like:
- adding points to the plane by coords
- clustering them
- telling you what can you do with it

## Definitions
We will call the bunch of points which are not too far from each other as _cluster_, and search of clusters - clustering. ALso we'll call _cloud_ group of points created by the command `gc`. As the command works, the point here are not too far from each other so they really look like cloud. We call _field_ the plane where all our points, clouds and clusters exists. This is our main space for work.

## Command description 
The brief version of the list can be got just by typing `help` to the program. Used aruments are:
- `d` - real number 
- `s` - string
- `i` - integer number

### Core commands
  `binary d`
  creates incidence matrix with the rule: 1 if points are closer than d, 0 if farther. This matrix is used in dbscan and wave algorithms
  
  `exit`
	stops the program
  
  `help`
	shows the list of supported commands
  
  `log i`
	switches writing logs: i == 0 starts writing, i == 1 stops.
	No parameter shows current state of writing logs
  
  `matrix` sets field to state readonly and prepares it to be analyzed
  `printf s` prints the content of the field to file called file_name

  
### Cloud generation
  `gc d1 d2 d3 d4 i`
	generates cloud on the field. Center of the cloud is point (d1, d2), that means most of points will be near that place. Dispersion of the cloud is d3 and d4 by x and y. There will be i points. By the order of creating every cloud gets its own number
  
  `buffer s`
  Buffer helps to copy and paste clouds over the field. s is for operation. It can be:
  
     `unload` unloads buffer to field
     `add i` adds cloud with number i to buffer
     `rotate d` rotates buffer to angle d
     `shift d1 d2` shifts buffer to vector (d1, d2)
     `zoom d` zooms buffer to coefficient d from its center
     `mirror` reflects the cloud like in mirror
     `erase` deletes all points from the buffer
     
### Analyzing algorithms
  `delaunay` creates Delaunay triangulation on the field
  
  `dbscan i1 i2` searches clusters in the field using dbscan algorithm with parameter i2 using incidences in search numbered i1
  
  `em i` searches clusters in the field using Expectation-Maximization-algorithm
  
  `forel` realises FOREL-tree algorithm
  
  `ha` runs hierarchical algorithm
  
  `histogram i` saves to histogram_x.txt and histogram_y.txt histogram of distribution the points in cloud. i is number of pieces in histogram
  
  `kmeans i` search i1 clusters in the field using k-means algorithm
  
  `kmcores i1 i2` search i1 clusters in the field using k-means algorithm with i2 cores
  
  `stree` searches minimal spanning tree for algorithm
  
  `wave i` searches clusters in the field using wave-clustering algorithm using incidences in search numbered i
