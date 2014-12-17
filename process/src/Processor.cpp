#include "Processor.h"

using namespace cv;

// Initialize everything 
void Processor::initialize(std::string imgFile, std::string styleFile, bool verby){
    verbose = verby;
    
    // load image
    if(verbose){
        std::cout << std::endl << "Reading image:  " << imgFile << std::endl;
    }

    // Read in image
    image = imread(imgFile, CV_LOAD_IMAGE_COLOR);    
    if(!image.data){
        std::cout << "No image data :(" << std::endl;
        return;
    }
    Size imgSize = image.size();
 
    // Load style
    if(verbose){
        std::cout << "  Image size: " << imgSize << std::endl << std::endl;
        std::cout << "With style: " << styleFile << std::endl;
    }
    YAML::Node styleNode = YAML::LoadFile(styleFile);
    canvStyle = styleNode.as<CanvasStyle>();
    
    canvas.width = imgSize.width * canvStyle.canvasScale;
    canvas.height = imgSize.height * canvStyle.canvasScale;

    // Initialize layers
    for(int i = 0; i < canvStyle.layers.size(); ++i){
        Layer l;
        canvas.layers.push_back(l);
    }

    // Blur images by avgBrushWidth
    for(int i = 0; i < canvStyle.layers.size(); ++i){
        
        int kernelSize = (int)(4.0 * canvStyle.layers[i].avgBrushWidth/2.0); 
        // Needs to be positively odd for Gaussian
        if(kernelSize/2.0 == std::floor(kernelSize/2.0))
            kernelSize ++;

        Mat blimage = image.clone();
        // Absolutely do not blur the top layer; use the original image
        if(i < canvStyle.layers.size() -1)
            GaussianBlur(image, blimage, Size(kernelSize, kernelSize), 0, 0, BORDER_DEFAULT);
     
        if(verbose){
            std::cout << "Blurring image for canvas " << i << std::endl;
            displayImage(blimage, "blurred");
        }   

        blurimages.push_back(blimage);
    }

    // Get Sobel gradients
    for(int i = 0; i < canvStyle.layers.size(); ++i){
        Mat grayimg;
        cvtColor(blurimages[i], grayimg, CV_RGB2GRAY);    
        Mat gradX;
        Mat gradY;
        Mat grad;

        int kernelsize = 3;

        Sobel(grayimg, gradX, CV_32F, 1, 0, kernelsize, 1.0, 0, BORDER_DEFAULT); 
        Sobel(grayimg, gradY, CV_32F, 0, 1, kernelsize, 1.0, 0, BORDER_DEFAULT);
        magnitude(gradX, gradY, grad);
    
        gradimages.push_back(grad);
        gradXimages.push_back(gradX);
        gradYimages.push_back(gradY);
    }
    
    srand(time(NULL));

    if(verbose){
        std::cout << "Reading canvas -done- " << std::endl;
        std::cout << "  canvasScale: " << canvStyle.canvasScale << std::endl;
        std::cout << "  canvasSize: " << canvas.width << " x " << canvas.height << std::endl;
        std::cout << "  num layers: " << canvas.layers.size() << std::endl;     
        std::cout << std::endl << "End initialization............" << std::endl << std::endl;
    }
}

// If verbose flag is not passed in, assume debugging info is off
void Processor::initialize(std::string imgFile, std::string styleFile){
    initialize(imgFile, styleFile, false);
}

// Process each layer, building its strokes
void Processor::processImage() {    
    for(int i = 0; i < canvas.layers.size(); ++i){
        LayerStyle& lstyle = canvStyle.layers[i];
        Layer& layer = canvas.layers[i];
        if(verbose){
            std::cout << "On Layer " << i << std::endl;
            std::cout << "  regenWidth: " << lstyle.regenWidth << std::endl;
            std::cout << "  regenMaskWidth: " << lstyle.regenMaskWidth << std::endl;
            std::cout << "  avgBrushWidth: " << lstyle.avgBrushWidth << std::endl;
            std::cout << "  varBrushWidth: " << lstyle.varBrushWidth << std::endl;
            std::cout << "  opacity: " << lstyle.opacity << std::endl;
            std::cout << "  strengthThreshold: " << lstyle.strengthThreshold << std::endl;
            std::cout << "  strengthNeighborhood: " << lstyle.strengthNeighborhood << std::endl;             
        }      

        // get stroke positions, opacities, widths
        buildStrokes(layer, lstyle, i);
        // get stroke angles
        angleStrokes(layer, lstyle, i);
        // clip stroke lengths along lines
        clipStrokes(layer, lstyle, i);
        // get stroke colors
        colorStrokes(layer, lstyle, i);

        if(verbose)
            std::cout << std::endl;
    }
}

// Output a YAML file with the entire canvas
void Processor::saveToFile(std::string outFile){
    if(verbose){
        std::cout << "writing to file... " << outFile << std::endl;
    }
    YAML::Emitter yout;
    YAML::convert<Canvas> yconv;
    YAML::Node canvasNode = yconv.encode(canvas);
    yout << canvasNode;
    
    std::ofstream fout; 
    fout.open(outFile.c_str());
    fout << yout.c_str();
    fout.close();
    if(verbose){
        std::cout << "finished writing" << std::endl;
    }
}


// Get anchors, set width + opacity
void Processor::buildStrokes(Layer& layer, LayerStyle& lstyle, int lid){
    cv::Mat& blurimg = blurimages[lid];
    // number of estimated necessary iterations is a factor of image size
    int k = .6 * canvas.width * canvas.height; 
    // stop iterating if you got stopthresh consecutive fails
    int stopthresh = k * .005;
    int countstop = 0;    
    double wrhalf = lstyle.regenWidth / 2.0;

    if(verbose){
        std::cout << "buildingStrokes!" << std::endl;
        std::cout << "  k: " << k << std::endl;
        std::cout << "  stopthresh: " << stopthresh << std::endl;
    }

    // Create a mask to store placed brushstrokes and allowed placement locations
    // On higher layers, only place strokes near high frequency areas
    Scalar s = 0;
    Mat maskimg(canvas.height, canvas.width, CV_8UC1, s);     
    Size maskSize = maskimg.size();
    // regenMaskWidth == 0 only for the bottom layer, meaning that you should fill the whole layer
    // Otherwise create a mask around permitted regions
    if (lstyle.regenMaskWidth > .00000001){
        createRegenMask(maskimg, lid, lstyle);        
    }

    // For k iterations (or until stopped), try placing a stroke
    // If there is already a stroke in its neighborhood, discard it and try again
    for(int i = 0; i < k; ++i){
        if(countstop > stopthresh){
            if(verbose){
                std::cout << "breaking: reached stopthresh" << std::endl;
            }
            break;
        }
        
        // random point
        int r = rand()%maskSize.height;
        int c = rand()%maskSize.width;
        
        // the boundaries of its neighborhood
        int rs = std::max(0, (int)(r-wrhalf));
        int re = std::min((int)(r+wrhalf), maskSize.height);
        int cs = std::max(0, (int)(c-wrhalf));
        int ce = std::min((int)(c+wrhalf), maskSize.width);

        if(verbose){
            std::cout << "test position: " << r << ", " << c << std::endl;
            std::cout << "  r: [" << rs << ", " << re << "]" << std::endl;
            std::cout << "  c: [" << cs << ", " << ce << "]" << std::endl;
        }
        
        // See if there's any strokes already in the area, or whether it's masked
        if(countNonZero(maskimg(Range(rs, re), Range(cs, ce))) > 0){
            countstop++;
            if(verbose){
                std::cout << "  rejected" << std::endl;
                std::cout << "  countstop: " << countstop << std::endl;
            }
            continue;
        }
        
        // valid position
    
        countstop = 0;
        maskimg.at<uchar>(r, c) = 255;
        
        Brushstroke b;
        // create a stroke with dummy values for things other than position, width, and opacity
        makeDummyStroke(b, Point2d(c, r), lstyle.avgBrushWidth, lstyle.varBrushWidth, lstyle.opacity);

        if(verbose){
            std::cout << "  New stroke: " << std::endl;
            std::cout << "      anchor: " << b.anchor << std::endl;
            std::cout << "      width: " << b.width << std::endl;
            std::cout << "      opacity: " << b.opacity << std::endl;
        }

        layer.strokes.push_back(b);
    }

    if(verbose){
        displayImage(maskimg, "Before postprocessing");
        std::cout << "-------------Postprocessing stroke generation----------------" << std::endl;
    }

    // Once you tried and stopped the random stroke-placements, go through the image 
    // in scanline fashion and fill in the holes
    // Obviously if the random stroke-placement was stopped too soon, this will make the image
    // appear too ... gridded
    for(int r = 0; r < maskSize.height; ++r){
        for(int c = 0; c < maskSize.width; ++c){

            int rs = std::max(0, (int)(r-wrhalf)); 
            int re = std::min((int)(r+wrhalf), maskSize.height);
            int cs = std::max(0, (int)(c-wrhalf));
            int ce = std::min((int)(c+wrhalf), maskSize.width);

            if (countNonZero(maskimg(Range(rs, re), Range(cs, ce))) >= 1){
                continue;
            }
            // valid point

            maskimg.at<uchar>(r,c) = 255; 

            Brushstroke b;
            makeDummyStroke(b, Point2d(c, r), lstyle.avgBrushWidth, lstyle.varBrushWidth, lstyle.opacity);

            if(verbose){
                std::cout << "  New stroke: " << std::endl;
                std::cout << "      anchor: " << b.anchor << std::endl;
                std::cout << "      width: " << b.width << std::endl;
                std::cout << "      opacity: " << b.opacity << std::endl;
            }

            layer.strokes.push_back(b);
        }
    }

    if(verbose){
        std::cout << "Done building strokes..." << std::endl;
        displayImage(maskimg, "Brush anchors");
    }
}

// Determine the angle from the local gradients
void Processor::angleStrokes(Layer& layer, LayerStyle& lstyle, int lid){
    if(verbose){
        std::cout << "Angling strokes................" << std::endl;

    }

    Mat& grad = gradimages[lid];
    Mat& gradX = gradXimages[lid];
    Mat& gradY = gradYimages[lid];

    double scale = canvStyle.canvasScale; 
    Mat pix;
    for(int i = 0; i < layer.strokes.size(); ++i){
        Brushstroke& stroke = layer.strokes[i];
        Point2d anch = Point2d(stroke.anchor.x/scale, stroke.anchor.y/scale);

        getRectSubPix(gradX, Size(1, 1), anch, pix);
        float bgradX = pix.at<float>(0, 0);
        getRectSubPix(gradY, Size(1, 1), anch, pix);
        float bgradY = pix.at<float>(0, 0);
        getRectSubPix(grad, Size(1, 1), anch, pix);
        float bgrad = pix.at<float>(0, 0);        

        // angle is perpendicular to gradient direction
        stroke.angle = atan2(bgradY, bgradX) + PI/2;
        // and strength relative to gradient strength
        stroke.strength = bgrad;        

        if(verbose){
            std::cout << "At stroke " << stroke.anchor << std::endl;
            std::cout << "  grad: " << bgradX << ", " << bgradY << std::endl;
            std::cout << "  angle: " << stroke.angle << std::endl;
            std::cout << "  strength: " << stroke.strength << std::endl;
        }

    }

    if(verbose){
        std::cout << "finished initial angling" << std::endl;
    }

    // Not yet supported, but hypothetically ditch 'weak' strokes in favor of interpolation 
    // from strong neighbors
    //interpAngles(layer, lstyle, lid);
}

// Clip (cut) the length of strokes when they cross a line
void Processor::clipStrokes(Layer& layer, LayerStyle& lstyle, int lid){
    cv::Mat& blurimg = blurimages[lid];
    if(verbose){
        std::cout << "Clipping strokes..................." << std::endl;
    }

    // edges detected based on Sobel-filtered gradients of the intensity image
    Mat& grad = gradimages[lid];

    double scale = canvStyle.canvasScale;
    double maxLength = lstyle.maxBrushLength/2.0/scale/scale;     
    double diffthresh = -lstyle.clipThreshold;
    
    if(verbose){
        Mat sobelimg;
        grad.convertTo(sobelimg, CV_8U, 1);
        displayImage(sobelimg, "sobel gradient");
    }

    // For each stroke... 
    for(int i = 0; i < layer.strokes.size(); ++i){
        // Initialize endpoints to anchor point
        Brushstroke& stroke = layer.strokes[i];
        Point2d anch = Point2d(stroke.anchor.x/scale, stroke.anchor.y/scale);
        Point2d x1 = Point2d(anch);
        Point2d x2 = Point2d(anch);
        Point2d tempx = Point2d(anch);
        Mat pix;
        getRectSubPix(grad, Size(1, 1), x1, pix);
        float oldSample = pix.at<float>(0, 0);
        float newSample = std::numeric_limits<float>::max();
        double dirX = cos(stroke.angle);
        double dirY = sin(stroke.angle);        

        bool keepgoing = true;
        // Take unit steps in a direction, stopping when you cross a line
        // Line is crossed when the gradient decreases
        while (keepgoing){
            tempx.x = x1.x + dirX;
            tempx.y = x1.y + dirY;

            if(distCanvas(tempx, anch) > maxLength)
                break;

            getRectSubPix(grad, Size(1,1), tempx, pix);
            newSample = pix.at<float>(0, 0);
            if(newSample - oldSample < diffthresh)
                break;

            x1.x = tempx.x;
            x1.y = tempx.y;
            oldSample = newSample;

        }

        // now do x2
        keepgoing = true;
        getRectSubPix(grad, Size(1,1), x2, pix);
        oldSample = pix.at<float>(0, 0);
        newSample = std::numeric_limits<float>::max();
        tempx.x = x2.x;
        tempx.y = x2.y;        

        while(keepgoing){
            tempx.x = x2.x - dirX;
            tempx.y = x2.y - dirY;

            if(distCanvas(tempx, anch) > maxLength)
                break;

            getRectSubPix(grad, Size(1,1), tempx, pix);
            newSample = pix.at<float>(0, 0);
            if(newSample - oldSample < diffthresh)
                break;

            x2.x = tempx.x;
            x2.y = tempx.y;
            oldSample = newSample;
        }   

        stroke.length1 = max(.5, distCanvas(x1, anch));
        stroke.length2 = max(.5, distCanvas(x2, anch));

        bool superverbose = false;
        if(superverbose){
            Mat sobelimg;
            grad.convertTo(sobelimg, CV_8U, 1);
            line(sobelimg, x1, x2, Scalar(255, 0, 0), 1);
            displayImage(sobelimg, "stroke");
        }

        if(verbose){
            std::cout << "for stroke at " << stroke.anchor << std::endl;
            std::cout << "  length 1: " << stroke.length1 << std::endl;
            std::cout << "  length 2: " << stroke.length2 << std::endl;

        }


    }
    if(verbose){
        std::cout << "Done clipping strokes..............." << std::endl;
    }
}

// Give the stroke a color based on image color at anchor point
void Processor::colorStrokes(Layer& layer, LayerStyle& lstyle, int lid){
    cv::Mat& blurimg = blurimages[lid];
    // for now, just use the color at the anchor point

    if(verbose){
        std::cout << "Started coloring strokes" << std::endl;
    }

    Vec3b col; 
    Vec3d colVar = lstyle.colorVariance;
    RNG rng(0xFFFFFFFF);
    double scale = canvStyle.canvasScale;
    for(int i = 0; i < layer.strokes.size(); ++i){
        Brushstroke& stroke = layer.strokes[i];
        // use blurred image when getting color 
        col = blurimg.at<Vec3b>(Point(stroke.anchor.x / scale, stroke.anchor.y / scale));
        stroke.color = BGR_TO_RGBDOUBLE(col);

        Vec3d colOffset;
        colOffset[0] = rng.uniform(-colVar[0], colVar[0]);
        colOffset[1] = rng.uniform(-colVar[1], colVar[1]);
        colOffset[2] = rng.uniform(-colVar[2], colVar[2]);

        stroke.color += colOffset;
        stroke.color[0] = CLAMP(stroke.color[0], 0, 1);
        stroke.color[1] = CLAMP(stroke.color[1], 0, 1);
        stroke.color[2] = CLAMP(stroke.color[2], 0, 1);

        if(verbose){
            std::cout << " Set color at stroke " << stroke.anchor << " to " << stroke.color << std::endl;
        }
    }
    if(verbose){
        std::cout << "Finished coloring strokes" << std::endl;
    }
}

void Processor::makeDummyStroke(Brushstroke& stroke, Point2d ankh, double avgWb, double dWb, double opac){
    stroke.anchor = ankh;
    stroke.width = avgWb + (2.0*(double)rand()/(RAND_MAX) - 1.0)*dWb;
    stroke.opacity = opac; 

    // dummy values for other parameters
    stroke.angle = .78;
    stroke.strength = 0;
    stroke.length1 = 2;
    stroke.length2 = 2;
    stroke.color = Vec3d(.5, .5, .5);

}

// Create a mask around high-frequency areas of the blurred image per layer
void Processor::createRegenMask(cv::Mat& mask, int lid, LayerStyle& lstyle){
    Mat threshimg; 
    Mat& grad = gradimages[lid];

    double rmaskwidth = lstyle.regenMaskWidth;
    double threshval = lstyle.strengthThreshold; 
    if((int)(threshval/2.0) == threshval/2.0)
        threshval ++;
 
    if(verbose){
        std::cout << "  thresholding..." << std::endl;
        std::cout << "  threshvalue: " << threshval << std::endl;
    }

    threshold(grad, threshimg, threshval, 255, 1);
    threshimg.convertTo(threshimg, CV_8U, 1);
    threshold(threshimg, threshimg, 20, 255, 0);

    // grow the high frequency area by an amount changed by regenMaskWidth
    rmaskwidth = max(rmaskwidth, 2.0);
    double erodekernel = rmaskwidth/2.0;
    if((int)(erodekernel/2.0) == erodekernel/2.0)
        erodekernel ++;
    if(verbose){
        displayImage(threshimg, "before erosion");
    }

    Mat erodelem = getStructuringElement(MORPH_ELLIPSE, Size(erodekernel, erodekernel));
    erode(threshimg, threshimg, erodelem);


    if(verbose){
        displayImage(threshimg, "mask before hole-filling");
    }

    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(threshimg, threshimg,  MORPH_OPEN, element);

    if(verbose){
        std::cout << "threshtype: " << threshimg.type() << std::endl;
        displayImage(threshimg, "mask after hole-filling");
    }


    resize(threshimg, mask, mask.size(), 0, 0, INTER_LINEAR);
    // set mask value

} 

// factor in the scale factor when calculating distance
// Points passed-in are in image-scale; distance in canvas-scala
double Processor::distCanvas(Point2d& x1, Point2d& x2){
    double currdist = sqrt(((x1.x - x2.x)*(x1.x - x2.x)) + ((x1.y - x2.y)*(x1.y - x2.y)));
    return currdist * canvStyle.canvasScale*canvStyle.canvasScale;
}

void Processor::interpAngles(Layer& layer, LayerStyle& lstyle, int lid){
    // TO BE CONTINUED:
    // Eventually want to use an interpolation scheme for the angles;
    // Discard all angles calculated locally that fall under a threshold
    // value, and instead interpolate them from the "strong" remaining ones.

    // Keep track of "strong strokes"
    vector<Brushstroke> strongStrokes;
    for(int i = 0; i < layer.strokes.size(); ++i){
        Brushstroke& stroke = layer.strokes[i];
        if(stroke.strength > lstyle.strengthThreshold){
            strongStrokes.push_back(stroke);
        }
    }
    std::cout << "strong strokes: " << strongStrokes.size() << std::endl;

    int numStrong = strongStrokes.size();
    Scalar s = 0;
 
    Mat angdata(canvas.height, canvas.width, CV_8UC1, s);
    Mat angMask(canvas.height, canvas.width, CV_8UC1, s);
    Mat angresult(canvas.height, canvas.width, CV_8UC1, s); 

    for(int i = 0; i < numStrong; ++i){
    }
}


// Display an image in a window with the passed-in name, wait for user key
void Processor::displayImage(cv::Mat& img, std::string windowName){
    namedWindow(windowName, WINDOW_NORMAL);
    imshow(windowName, img);
    waitKey(0);
    destroyWindow(windowName);
}




