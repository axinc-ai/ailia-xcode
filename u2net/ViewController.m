//
//  ViewController.m
//  u2net
//
//  Created by 客野一樹 on 2020/11/05.
//  Copyright © 2020 ax Inc. All rights reserved.
//

#import "ViewController.h"

#include "ailia.h"
#include <vector>

#define PRINT_ERR printf
#define PRINT_OUT printf

@interface ViewController ()

@end

@implementation ViewController

/*
static int load_image(cv::Mat& image, cv::Size& src_size, const char* path, cv::Size scaled_size)
{
    cv::Mat oimg = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (oimg.empty()) {
        PRINT_ERR("\'%s\' not found\n", path);
        return -1;
    }

    src_size = cv::Size(oimg.cols, oimg.rows);
    transform(oimg, image, scaled_size);

    return 0;
}



static void normalize(const cv::Mat& simg, cv::Mat& dimg)
{
    float min = FLT_MAX;
    float max = FLT_MIN;
    float* sdata = (float*)simg.data;
    int size = simg.rows*simg.cols*simg.channels();

    for (int i = 0; i < size; i++) {
        min = std::min(min, sdata[i]);
        max = std::max(max, sdata[i]);
    }

    dimg = cv::Mat(simg.rows, simg.cols, CV_8UC1);
    unsigned char* ddata = (unsigned char*)dimg.data;
    for (int i = 0; i < size; i++) {
        ddata[i] = ((sdata[i]-min) / (max-min))*255.0f;
    }

    return;
}
 */


- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.

    UIImage *img = [UIImage imageNamed:@"input.png"];


    // First get the image into your data buffer
    CGImageRef image = [img CGImage];
    NSUInteger width = CGImageGetWidth(image);
    NSUInteger height = CGImageGetHeight(image);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    unsigned char *rawData = malloc(height * width * 4);
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(rawData, width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    CGContextRelease(context);

    // Now your rawData contains the image data in the RGBA8888 pixel format.
    int xx=0;
    int yy=0;
    
    int byteIndex = (bytesPerRow * yy) + xx * bytesPerPixel;
    //red = rawData[byteIndex];
    //green = rawData[byteIndex + 1];
    //blue = rawData[byteIndex + 2];
    //alpha = rawData[byteIndex + 3];
    
    for(int i=0;i<width*height*4;i++){
        //printf("%d ",rawData[i]);   //BGRA
        rawData[i]+=20;
    }
    
    
    
    NSString* path1 = [[NSBundle mainBundle] pathForResource:@"u2net_opset11" ofType:@"prototxt"];
    NSString* path2 = [[NSBundle mainBundle] pathForResource:@"u2net_opset11" ofType:@"onnx"];

        // net initialize
        struct AILIANetwork *net;
        int env_id = AILIA_ENVIRONMENT_ID_AUTO;
        int status = ailiaCreate(&net, env_id, AILIA_MULTITHREAD_AUTO);
        if (status != AILIA_STATUS_SUCCESS) {
            PRINT_ERR("ailiaCreate failed %d\n", status);
            return;
        }

        AILIAEnvironment *env_ptr = nullptr;
        status = ailiaGetSelectedEnvironment(net, &env_ptr, AILIA_ENVIRONMENT_VERSION);
        if (status != AILIA_STATUS_SUCCESS) {
            PRINT_ERR("ailiaGetSelectedEnvironment failed %d\n", status);
            ailiaDestroy(net);
            return;
        }

    //    PRINT_OUT("env_id: %d\n", env_ptr->id);
        PRINT_OUT("env_name: %s\n", env_ptr->name);

        status = ailiaOpenStreamFile(net, [path1 cStringUsingEncoding:1]);
        if (status != AILIA_STATUS_SUCCESS) {
            PRINT_ERR("ailiaOpenStreamFile failed %d\n", status);
            PRINT_ERR("ailiaGetErrorDetail %s\n", ailiaGetErrorDetail(net));
            ailiaDestroy(net);
            return;
        }

        status = ailiaOpenWeightFile(net, [path2 cStringUsingEncoding:1]);
        if (status != AILIA_STATUS_SUCCESS) {
            PRINT_ERR("ailiaOpenWeightFile failed %d\n", status);
            ailiaDestroy(net);
            return;
        }

            // prepare input data
            //cv::Mat  input;
            //cv::Size src_size;
            //int status = load_image(input, src_size, image_path.c_str(), cv::Size(IMAGE_SIZE, IMAGE_SIZE));
            //if (status != AILIA_STATUS_SUCCESS) {
            //    return -1;
            //}

            AILIAShape input_shape;
            status = ailiaGetInputShape(net, &input_shape, AILIA_SHAPE_VERSION);
            if (status != AILIA_STATUS_SUCCESS) {
                PRINT_ERR("ailiaGetInputShape failed %d\n", status);
                return;
            }
        //    PRINT_OUT("input shape %d %d %d %d %d\n",input_shape.x, input_shape.y, input_shape.z, input_shape.w, input_shape.dim);
            int input_size = input_shape.x*input_shape.y*input_shape.z*input_shape.w*sizeof(float);

            AILIAShape output_shape;
            status = ailiaGetOutputShape(net, &output_shape, AILIA_SHAPE_VERSION);
            if (status != AILIA_STATUS_SUCCESS) {
                PRINT_ERR("ailiaGetOutputShape failed %d\n", status);
                return;
            }
        //    PRINT_OUT("output shape %d %d %d %d %d\n", output_shape.x, output_shape.y, output_shape.z, output_shape.w, output_shape.dim);
            int preds_size = output_shape.x*output_shape.y*output_shape.z*output_shape.w*sizeof(float);
            cv::Mat preds_ailia(output_shape.y, output_shape.x, CV_32FC1);

            // inference

                status = ailiaPredict(net, preds_ailia.data, preds_size, input.data, input_size);
                if (status != AILIA_STATUS_SUCCESS) {
                    PRINT_ERR("ailiaPredict failed %d\n", status);
                    return;
                }

            std::vector<int> new_shape = {(int)output_shape.y, (int)output_shape.x};
            preds_ailia.reshape(1, new_shape.size(), &new_shape[0]);
            status = save_result(preds_ailia, save_image_path.c_str(), src_size);
            if (status != AILIA_STATUS_SUCCESS) {
                return -1;
            }

            PRINT_OUT("Program finished successfully.\n");

        ailiaDestroy(net);
    
    
    
    
    
    
    
    //size_t width                    = CGImageGetWidth(image);
    //size_t height                   = CGImageGetHeight(image);
    //size_t bitsPerComponent         = CGImageGetBitsPerComponent(image);
    size_t bitsPerPixel             = CGImageGetBitsPerPixel(image);
    //size_t bytesPerRow              = CGImageGetBytesPerRow(image);

    CGColorSpaceRef colorspace      = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo         = CGImageGetBitmapInfo(image);
    CGDataProviderRef provider      = CGDataProviderCreateWithData(NULL, rawData, height * width * 4, NULL);

    CGImageRef newImageRef = CGImageCreate (
                              width,
                              height,
                              bitsPerComponent,
                              bitsPerPixel,
                              bytesPerRow,
                              colorspace,
                              bitmapInfo,
                              provider,
                              NULL,
                              false,
                              kCGRenderingIntentDefault
                              );

    UIImage *newImage   = [UIImage imageWithCGImage:newImageRef];

    UIImageView *imageView = [[UIImageView alloc] initWithImage:newImage];

     CGFloat pWidth = [UIScreen mainScreen].bounds.size.width;
     CGFloat pHeight = [UIScreen mainScreen].bounds.size.height;

     [imageView setCenter:CGPointMake(pWidth/2, pHeight/2)];

    [self.view addSubview:imageView];

}


@end
