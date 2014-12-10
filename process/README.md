General Algorithm
    (or so it would appear to be):

Goal:
Output a file with:
    - Canvas:
            - width
            - height
            - Vector <Brushstroke> layers[num layers]

            
    - Brushstroke:
            - Point(x,y) anchor
            - double Orientation (radians)
            - double Length 1 
            - double Length 2
            - double Width
            - double opacity (0-1)
            - Color color

1) Read in source image and 'style' config file (which defines parameters for things like the number of layers, brush width/layer, max length/layer, etc)... 

2) Initialize layers, each of size source * scale... 
    For each layer, have:
        - wb: brush width (maybe with some random variation?)
        - wr: regen width (determine how densely packed the brushstrokes are)
        - brush type (and texture...)
        - palette reduction (although may be part of render instead)
        - opacity? (or vary per brushstroke?)

3) For each layer... 
    i. Determine position of brushstrokes: 
        Find all "anchor points" of each brushstroke...
        This is a pseudorandom process (to limit artifacts), and attempts to 
        fill all areas of size wr x wr with at least one stroke. 
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

    ii. 
