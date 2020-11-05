//
//  ViewController.m
//  u2net
//
//  Created by 客野一樹 on 2020/11/05.
//  Copyright © 2020 ax Inc. All rights reserved.
//

#import "ViewController.h"

#include "ailia.h"

#define PRINT_ERR printf
#define PRINT_OUT printf

@interface ViewController ()

@end

@implementation ViewController

static void ailia_u2net(unsigned char *rgb,unsigned int width,unsigned int height){
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

    AILIAEnvironment *env_ptr = NULL;
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

        AILIAShape input_shape;
        status = ailiaGetInputShape(net, &input_shape, AILIA_SHAPE_VERSION);
        if (status != AILIA_STATUS_SUCCESS) {
            PRINT_ERR("ailiaGetInputShape failed %d\n", status);
            return;
        }
    //    PRINT_OUT("input shape %d %d %d %d %d\n",input_shape.x, input_shape.y, input_shape.z, input_shape.w, input_shape.dim);
        unsigned int input_size = input_shape.x*input_shape.y*input_shape.z*input_shape.w*sizeof(float);
        float *input_buf = (float*)malloc(input_size);
    
        preprocess(input_buf, input_shape.x, input_shape.y, rgb, width, height);
    
        AILIAShape output_shape;
        status = ailiaGetOutputShape(net, &output_shape, AILIA_SHAPE_VERSION);
        if (status != AILIA_STATUS_SUCCESS) {
            PRINT_ERR("ailiaGetOutputShape failed %d\n", status);
            return;
        }
    //    PRINT_OUT("output shape %d %d %d %d %d\n", output_shape.x, output_shape.y, output_shape.z, output_shape.w, output_shape.dim);
        int preds_size = output_shape.x*output_shape.y*output_shape.z*output_shape.w*sizeof(float);
        float *output_buf = (float*)malloc(preds_size);

        // inference

            status = ailiaPredict(net, output_buf, preds_size, input_buf, input_size);
            if (status != AILIA_STATUS_SUCCESS) {
                PRINT_ERR("ailiaPredict failed %d\n", status);
                return;
            }

    postprocess(rgb, width, height, output_buf, output_shape.x, output_shape.y);

        free(input_buf);
        free(output_buf);

        PRINT_OUT("Program finished successfully.\n");

    ailiaDestroy(net);
}

static int preprocess(float *dst,unsigned int dst_width,unsigned int dst_height,const unsigned char *src,unsigned int src_width,unsigned int src_height)
{
    unsigned char max_value=1;
    for(int y=0;y<src_height;y++){
        for(int x=0;x<src_width*4;x++){
            unsigned char value=src[y*src_width*4+x];
            if(max_value<value){
                max_value=value;
            }
        }
    }

    for(int y=0;y<dst_height;y++){
        for(int x=0;x<dst_width;x++){
            int sy=y*src_height/dst_height;
            int sx=x*src_width/dst_width;
            dst[y*dst_width*3+x*3+0]=(1.0*src[sy*src_width*4+sx*4+0]/max_value - 0.485) / 0.229;
            dst[y*dst_width*3+x*3+1]=(1.0*src[sy*src_width*4+sx*4+1]/max_value - 0.485) / 0.229;
            dst[y*dst_width*3+x*3+2]=(1.0*src[sy*src_width*4+sx*4+2]/max_value - 0.406) / 0.225;
        }
    }

    return 0;
}



static int postprocess(unsigned char *dst,unsigned int dst_width,unsigned int dst_height,const float *src,unsigned int src_width,unsigned int src_height)
{
    float min = FLT_MAX;
    float max = FLT_MIN;
    for(int y=0;y<src_height;y++){
        for(int x=0;x<src_width*4;x++){
            unsigned char value=src[y*src_width*4+x];
            if(min>value){
                min=value;
            }
            if(max<value){
                max=value;
            }
        }
    }
    
    for(int y=0;y<dst_height;y++){
        for(int x=0;x<dst_width;x++){
            int sy=y*src_height/dst_height;
            int sx=x*src_width/dst_width;
            dst[y*dst_width*4+x*4+3] = ((src[sy*src_width+sx]-min) / (max-min))*255.0f;
        }
    }

    return 0;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.

    UIImage *img = [UIImage imageNamed:@"input.png"];


    // First get the image into your data buffer
    CGImageRef image = [img CGImage];
    NSUInteger width = CGImageGetWidth(image);
    NSUInteger height = CGImageGetHeight(image);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    unsigned char *rawData = (unsigned char *)malloc(height * width * 4);
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(rawData, width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    CGContextRelease(context);

    // Now your rawData contains the image data in the RGBA8888 pixel format.
    //int xx=0;
    //int yy=0;
    
    //int byteIndex = (bytesPerRow * yy) + xx * bytesPerPixel;
    //red = rawData[byteIndex];
    //green = rawData[byteIndex + 1];
    //blue = rawData[byteIndex + 2];
    //alpha = rawData[byteIndex + 3];
    
    //for(int i=0;i<width*height*4;i++){
        //printf("%d ",rawData[i]);   //BGRA
    //    rawData[i]+=20;
    //}
    
    ailia_u2net(rawData,width,height);
    
    
    

    
    
    
    
    
    
    
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

    free(rawData);
}


@end
