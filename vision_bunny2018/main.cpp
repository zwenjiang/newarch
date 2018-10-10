#include <iostream>
#include "FileUtils.h"
#include "StringUtils.h"
#include "Cube.h"

//#include <cstdlib>
#include <libv4l2.h>
#include <linux/videodev2.h>
//#include <sys/ioctl.h>
#include <fcntl.h>


double getImageScaleFactor(const cv::Mat& frame,
                           const int      max_width,
                           const int      max_height)
{
    double width_scale = double(max_width) / frame.cols;
    double height_scale = double(max_width) / frame.rows;

    // Calculate overall scale factor, maintaining aspect ratio.
    // // If image already smaller, don't want to make it larger.
    double scale = std::min(width_scale, height_scale);
    scale = std::min(scale, 1.0);
    
    return scale;
}


void displayImage(const std::string& title,
                  const cv::Mat&     frame,
                  const int          x_coordinate,
                  const int          y_coordinate)
{
    cv::namedWindow(title.c_str(), cv::WINDOW_AUTOSIZE);
    cv::moveWindow(title.c_str(), x_coordinate, y_coordinate);
    cv::imshow(title.c_str(), frame);
}


int main(int argc, char **argv) {

    // Read param file
    if (argc < 2) {
        std::cout << "Please specify the params file.\n";
        return 1;
    }
    std::string params_file(argv[1]);
    paramsInput params;
    params.readFile(params_file);

    std::string image_source_file(params.getString("FILENAME", ""));
    if (!xero::file::exists(image_source_file)) {
        std::cout << "Input image file '" << image_source_file << "' does not exist.\n";
        return 1;
    }
    if (!xero::file::is_readable(image_source_file)) {
        std::cout << "Input image file '" << image_source_file << "' is unreadable.\n";
        return 1;
    }

    // Infer source type (image, video file, camera)
    const bool is_image = xero::string::endsWith(xero::string::downcase(image_source_file), ".jpg");
    const bool is_video = xero::string::endsWith(xero::string::downcase(image_source_file), ".mov") ||
                          xero::string::endsWith(xero::string::downcase(image_source_file), ".mp4");
    const bool is_camera = xero::string::startsWith(image_source_file, "/dev/video");
    if (!is_image && !is_video && !is_camera) {
        std::cout << "Unrecognized input source type '" << image_source_file << "'\n";
        return 1;
    }

    if (is_image) {
        // Open image
        cv::Mat frame_orig = cv::imread(image_source_file, cv::IMREAD_COLOR);
        if (!frame_orig.data) {
            std::cout << "Could not read image\n";
            return 1;
        }

        // Resize original image
        double scale = getImageScaleFactor(frame_orig, 640 /*max_width*/, 480 /*max_height*/);
        double width = scale * frame_orig.cols;
        double height = scale * frame_orig.rows;
        cv::Mat frame_orig_resized;
        cv::resize(frame_orig, frame_orig_resized, cv::Size(), scale, scale);
        
        // Show original image
        displayImage("Original", frame_orig_resized, 0, 0);

        // Apply: Blur
        enum BlurType {
            BOX, GAUSSIAN, MEDIAN, BILATERAL
        };
        cv::Mat frame_blur;
        BlurType blurType = BlurType::MEDIAN;
        double blurRadius = 15;
        int kernelSize = 2 * blurRadius + 1;
        cv::medianBlur(frame_orig_resized, frame_blur, kernelSize);
        displayImage("Blur", frame_blur, width, 0);

        // Apply: HSV threshold
        // Convert from BGR to HSV colorspace
        cv::Mat frame_HSV, frame_HSV_threshold;
        cvtColor(frame_blur, frame_HSV, cv::COLOR_BGR2HSV);
        // Filter based on HSV Range Values
        const int low_H =   0, high_H =  50;
        const int low_S =   0, high_S =  45;
        const int low_V = 167, high_V = 255;
        inRange(frame_HSV, cv::Scalar(low_H, low_S, low_V), cv::Scalar(high_H, high_S, high_V), frame_HSV_threshold);
        displayImage("HSV Threshold", frame_HSV_threshold, width*2, 0);

        // Apply: Erode
        cv::Mat frame_erode;
        cv::Mat cvErodeKernel;
        cv::Point cvErodeAnchor(-1, -1);
        int cvErodeIterations = 1;
        int cvErodeBorderType = cv::BORDER_DEFAULT;
        cv::Scalar cvErodeBorderValue(-1);
        cv::erode(frame_HSV_threshold, frame_erode, cvErodeKernel, cvErodeAnchor, cvErodeIterations, cvErodeBorderType, cvErodeBorderValue);
        displayImage("Erode", frame_erode, width*3, 0);
        
        // Apply: Dilate
        cv::Mat frame_dilate;
        cv::Mat cvDilateKernel;
        cv::Point cvDilateAnchor(-1, -1);
        int cvDilateIterations = 27;  // default Double
        int cvDilateBorderType = cv::BORDER_CONSTANT;
        cv::Scalar cvDilateBorderValue(-1);
        cv::dilate(frame_erode, frame_dilate, cvDilateKernel, cvDilateAnchor, cvDilateIterations, cvDilateBorderType, cvDilateBorderValue);
        displayImage("Dilate", frame_dilate, width*4, 0);

        // Blob detection
        double findBlobsCircularity[] = {0.0, 1.0};
        bool findBlobsDarkBlobs = false;
        cv::SimpleBlobDetector::Params params;
        params.filterByColor = 1;
        params.blobColor = (findBlobsDarkBlobs ? 0 : 255);
        params.minThreshold = 10;
        params.maxThreshold = 220;
        params.filterByArea = true;
        params.minArea = 7000;
        params.maxArea = 100000;  // Set to very large no. For some reason, it defaults to arbitrary limit that's too small.
        params.filterByCircularity = true;
        params.minCircularity = findBlobsCircularity[0];
        params.maxCircularity = findBlobsCircularity[1];
        params.filterByConvexity = false;
        params.filterByInertia = false;
        std::vector<cv::KeyPoint> keypoints;
        cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
        detector->detect(frame_dilate, keypoints);
        std::cout << "No. of detected blobs: " << keypoints.size() << std::endl;

        // Draw keypoints if only 1 found
        if (keypoints.size() == 1) {
            cv::Mat frame_with_keypoints;
            cv::drawKeypoints(frame_dilate, keypoints, frame_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
            displayImage("Dilate", frame_with_keypoints, width*4, 0);
        }
        

        // Detect and show cubes
        //Cube cube(frame, params);
        
        //std::cout << cube.getPosition() << std::endl;
        //cube.getPosition(Cube::detectionMode::CONTOURS);
        //cv::imshow("Cube", cube.showFrame());
        //cv::imshow("Camera", frame);
        //cv::imshow("Video", frame);
        
        cv::waitKey(0);
        
    } else if (is_video) {

        bool playVideo = true;
        char key;
        cv::VideoCapture capture(image_source_file);

        cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
        cv::moveWindow("Original", 0, 0);
        
        cv::Mat frame;
        while(capture.get(cv::CAP_PROP_POS_FRAMES)<capture.get(cv::CAP_PROP_FRAME_COUNT)) {

            if (!capture.isOpened()) {
                std::cout << "Could not read video\n";
                return 1;
            }
            if (playVideo) {
                capture >> frame;
                    
                cv::imshow("Original", frame);
                
                Cube cube(frame, params);
                
                //std::cout << cube.getPosition() << std::endl;
                //cube.getPosition(Cube::detectionMode::CONTOURS);
                //cv::imshow("Cube", cube.showFrame());
                //cv::imshow("Camera", frame);
                //cv::imshow("Video", frame);
            }
            key = cv::waitKey(15);
            if (key == 'p') playVideo = !playVideo;
            //sleep(10);
        }
        capture.release();

    } else if (is_camera) {

        const std::string cam_id_str = image_source_file.substr(strlen("/dev/video"));
        const int         cam_id     = std::stoi(cam_id_str);

        double exposure = params.getValue("CAMERA_EXPOSURE",-9001);
        int descriptor = v4l2_open(image_source_file.c_str(), O_RDWR);
        v4l2_control c;
        c.id = V4L2_CID_EXPOSURE_AUTO;
        c.value = int(params.getValue("CAMERA_EXPOSURE_AUTO", -9001));
        if(v4l2_ioctl(descriptor, VIDIOC_S_CTRL, &c) == 0)
            std::cout << "success";
        /*c.id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
          c.value = 0;
          if (v4l2_ioctl(descriptor, VIDIOC_S_CTRL, &c) == 0)
          std::cout << "success";*/
        
        c.id = V4L2_CID_EXPOSURE_ABSOLUTE;
        c.value = exposure;
        if (v4l2_ioctl(descriptor, VIDIOC_S_CTRL, &c) == 0)
            std::cout << "success";
        
        bool playVideo = true;
        char key;
        cv::VideoCapture capture(cam_id);

        cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
        cv::moveWindow("Original", 0, 0);
        
        cv::Mat frame;
        while (1) {
            
            if (!capture.isOpened()) {
                std::cout << "Could not read camera\n";
                return 1;
            }
            if (playVideo) {
                capture >> frame;
                
                cv::imshow("Original", frame);
                
                Cube cube(frame, params);
            }
            key = cv::waitKey(1);
            if (key == 'p') playVideo = !playVideo;
        }
        capture.release();
        
    } else {
        assert(0);
    }
      
    return 0;
}
