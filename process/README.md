## General Overview

### Goal:

Output a file with:

- Canvas:
    - width
    - height
    - Vector <Brushstroke> layers[num layers]
            
- Brushstroke:
    - Point(x,y) Anchor
    - double Angle (radians)
    - double Length 1 
    - double Length 2
    - double Width
    - double Opacity (0-1)
    - Color Color
    - Texture Texture
    - Strength 

### Algorithm

1. Read in source image and 'style' config file (which defines parameters for things like the number of layers, brush width/layer, max length/layer, etc)... 

2. Initialize layers, each of size source * scale... 
For each layer, have:
    - wb: brush width (maybe with some random variation?)
    - dwb: max variation in width
    - wr: regen width (determine how densely packed the brushstrokes are)
    - brush type (and texture...)
    - palette reduction (although may be part of render instead)
    - opacity? (or vary per brushstroke?)

3. For each layer... 
  1. Determine position of brushstrokes: 
        - Find all "anchor points" of each brushstroke...
        - This is a pseudorandom process (to limit artifacts), and attempts to fill all areas of size wr x wr with at least one stroke. 
        - For k iterations... 
            - choose random (x,y) on the canvas
            - if there is no anchor in the wr x wr space... create a new brushstroke with
                (x,y) as its anchorpoint... 
            - else... choose new point? 
            - When k is done, or when you've been unable to place your point p consecutive
                times, then... check whether your canvas is fully covered? 
                - I guess this should be done by scanning... check each wr x wr grid
                    in scanline method and see if it's empty... if yes, place your 
                    point there... 
        - Caveat for when not in bottom layer:
            - Brushstroke regeneration area limited to areas near high frequency data... 
            - Possible way to accomplish this is to find the high frequency data and create a mask, and discard any points that land outside this. This seems foolish, though, because clearly with the highest layers you'll be discarding the majority of your data, since the high frequency area will (presumably) be a small fraction of the image. Possible improvement is by determining a mapping algorithm that maps a point outside the permissable region into it somehow. 
  2. Width parameter
        - Is a function of the layer width + some random variance
  3. Opacity parameter
        - For now, constant per layer... with lower opacity higher up
        - Eventually, may want to implement a way to have opacity vary based on the importance of the stroke and its layering (e.g. if there's already multiple brushstrokes covering the same area, probably shouldn't be completely opaque)
  4. Texture parameter
        - Not applicable to process
  5. Angle
        - To determine orientation... 
        - First (on your layer which is blurred by a gaussian kernel with width proportional to wb)... estimate gradients with a Sobel filter. 
        - Brushstrokes which fall on gradients greater than some stylistically determined threshhold and not near other "strong" brushstrokes are marked as strong... 
            - How do you determine strength of neighbors? Maybe would be beneficial to have a nearest-neighbor graph or something
        - Something something radial basis function... which allows you to interpolate gradients from the 'strong' brushstrokes... and then angle = atan(ygrad, xgrad) + PI/2
        - It seems that as a general rule, rbfs are more or less weighted sums of 'training data' based on the distance from your known point to your unknown point... 

        - For now though... it may be better to just use the gradient at that location, and then add that functionality in later. 
  6. Strength... 
        - If using the rbf method, a brushstroke is marked as 'strong' iff the gradient at the anchor point is above some parameter-defined threshhold
  7. Length 1, 2
        - They have a thing called 'edge clipping'... don't allow a brushstroke to cross an edge. 
        - So first, find all edges: Run canny edge detection on all your (blurred(unless top)) layers to get your edges. 
        - Clip the brushstrokes to the edges according to "Processing Images and Video for an Impressionist Effect":
            - Start at anchor cx, cy
            - "Grow" the line in the orientation direction until maxLength is reached or an edge is detected (an edge is considered found if the magnitude of the gradient decreases in the direction of the stroke being grown)
            - More specifically, with (x1, y1) being an endpoint:
                a. Set (x1, y1) to (cx, cy)
                b. "bilinearly sample" the Sobel filtered intensity image at (x1, y1) (with Sobel value = magnitude of the gradient) and set lastSample = this value
                c. set (tempx, tempy) = (x1 + dirx, y1+diry), taking a unit step in the orientation direction
                d. if (dist(x1, y1), (tempx, tempy)) > length of stroke / 2, stop
                e. bilinearly sample the Sobel image at (tempx, tempy)... set newSample to this value
                f. if (newSample < lastSample), detected edge and stope
                g. set (x1, y1) to (tempx, tempy)
                h. set lastSample to newSample
                i. go to step c.
