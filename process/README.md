## General Overview

### Goal:

Process an image and ultimately output a file with:

- Canvas:
    - width
    - height
    - Vector <Brushstroke> layers[num layers]
            
- Brushstroke:
    - Point2d anchor: Position of stroke on canvas (x, y)
    - double angle: Orientation of stroke (radians)
    - double length1: Length along angle
    - double length2: Length opposite angle
    - double width: Width of stroke
    - double opacity: Opacity of stroke (0-1)
    - Vec3d color: RGB color of stroke
    - double strength: In progress 

### Algorithm

1. Read in source image and 'style' config file (which defines parameters for things like the number of layers, brush width/layer, max length/layer, etc)... 

2. Initialize layers, each of size source * scale... 
  1. For each layer but the top, create a blurred image with kernel proportional to brushstroke width
  2. Calculate the Sobel gradients on the blurred image
3. For each layer... 
  1. Determine position of brushstrokes: 
        - Find all "anchor points" of each brushstroke...
        - This is a pseudorandom process (to limit artifacts), and attempts to fill all areas of size regenWidth*regenWidth with at least one stroke. 
        - For k iterations... 
            - choose random (x,y) on the canvas
            - if there is no anchor in the regenWidth*regenWidth space, then create a new brushstroke with
                (x,y) as its anchorpoint... 
            - else... choose new point 
            - When k is done, or when you've been unable to place your point stopthresh consecutive
                times, then use scanline procedure to ensure that the canvas is appropriately covered
                - Check each regenWidth*regenWidth area in scanline procedure and, if it's empty of brush-anchors and                     in the target area, create a brushstroke there
        - Caveat for when not in bottom layer:
            - Brushstroke regeneration area limited to areas near high frequency data... 
            - To accomplish this, create a mask around high-frequency areas (plus some lee-way determined by regenMaskWidth and disallow any points to be generated outside of it
  2. Width parameter per stroke
        - Is a function of the layer width + some random variance
  3. Opacity parameter per stroke
        - Constant variable per layer
  4. Angle
        - To determine orientation... 
        - First (on your layer which is blurred by a gaussian kernel with width proportional to avgBrushWidth)... estimate gradients with a Sobel filter. 
        - Set the angle perpendicular to the gradient direction (i.e. the direction of strongest change)
        - For future work, perhaps angles along "weak" gradients should be discarded, and the value interpolated from neighboring strokes
  6. Strength... 
        - For future work, angles will be interpolated if strength (determined by gradient at the stroke) is under a threshold value
  7. Lengths 1, 2
        - Strokes are cut using an edge-clipping algorithm that prevent them from crossing lines in the image 
        - A line is found if the Sobel-gradient magnitudes drop past a given threshold between two sampled values
        - Clip the brushstrokes to the edges according to "Processing Images and Video for an Impressionist Effect":
            - Start at anchor cx, cy
            - "Grow" the line in the orientation direction until maxLength is reached or an edge is detected (an edge is considered found if the magnitude of the gradient decreases in the direction of the stroke being grown)
            - More specifically, with (x1, y1) being an endpoint:
    1. Set (x1, y1) to (cx, cy)
    2. "bilinearly sample" the Sobel filtered intensity image at (x1, y1) (with Sobel value = magnitude of the gradient) and set lastSample = this value
    3. set (tempx, tempy) = (x1 + dirx, y1+diry), taking a unit step in the orientation direction
    4. if (dist(x1, y1), (tempx, tempy)) > length of stroke / 2, stop
    5. bilinearly sample the Sobel image at (tempx, tempy)... set newSample to this value
    6. if (newSample < lastSample), detected edge and stope
    7. set (x1, y1) to (tempx, tempy)
    8. set lastSample to newSample
    9. go to step c.
  8. Color...
        - Take the color in the blurred image at the anchor point, plus some parameterized random color jitter
