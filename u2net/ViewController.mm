//
//  ViewController.m
//  u2net
//
//  Created by ax Inc. on 2020/11/05.
//  Copyright © 2020 ax Inc. All rights reserved.
//

#import "ViewController.h"

#include "ailia.h"
#include <vector>
#include <algorithm>

#define PRINT_ERR printf
#define PRINT_OUT printf

#define LOW_MEMORY_MODE

//#define MIDAS

@interface ViewController ()

@end

@implementation ViewController

static void ailia_u2net(unsigned char *rgb,unsigned int width,unsigned int height){
#ifdef MIDAS
    NSString* path1 = [[NSBundle mainBundle] pathForResource:@"midas_v21_small" ofType:@"prototxt"];
    NSString* path2 = [[NSBundle mainBundle] pathForResource:@"midas_v21_small" ofType:@"onnx"];
#else
    NSString* path1 = [[NSBundle mainBundle] pathForResource:@"u2net_opset11" ofType:@"prototxt"];
    NSString* path2 = [[NSBundle mainBundle] pathForResource:@"u2net_opset11" ofType:@"onnx"];
#endif
    
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

    PRINT_OUT("env_name: %s\n", env_ptr->name);

#ifdef LOW_MEMORY_MODE
    status = ailiaSetMemoryMode(net,AILIA_MEMORY_REDUCE_CONSTANT | AILIA_MEMORY_REDUCE_INTERSTAGE); //low memory mode
    if (status != AILIA_STATUS_SUCCESS) {
        PRINT_ERR("ailiaSetMemoryMode failed %d\n", status);
        ailiaDestroy(net);
        return;
    }
#endif
    
    //prototxt is optional
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

#ifdef MIDAS
    AILIAShape set_input_shape;
    set_input_shape.x = 256;
    set_input_shape.y = 256;
    set_input_shape.z = 3;
    set_input_shape.w = 1;
    set_input_shape.dim = 4;
    status = ailiaSetInputShape(net, &set_input_shape, AILIA_SHAPE_VERSION);
    if (status != AILIA_STATUS_SUCCESS) {
        PRINT_ERR("ailiaSetInputShape failed %d\n", status);
        return;
    }
#endif
    
    AILIAShape input_shape;
    status = ailiaGetInputShape(net, &input_shape, AILIA_SHAPE_VERSION);
    if (status != AILIA_STATUS_SUCCESS) {
        PRINT_ERR("ailiaGetInputShape failed %d\n", status);
        return;
    }

    unsigned int input_size = input_shape.x*input_shape.y*input_shape.z*input_shape.w*sizeof(float);
    std::vector<float> input_buf = std::vector<float>(input_size/sizeof(float));
    PRINT_OUT("input shape %d %d %d %d %d\n", input_shape.x, input_shape.y, input_shape.z, input_shape.w, input_shape.dim);

    preprocess(&input_buf[0], input_shape.x, input_shape.y, rgb, width, height);
    
    AILIAShape output_shape;
    status = ailiaGetOutputShape(net, &output_shape, AILIA_SHAPE_VERSION);
    if (status != AILIA_STATUS_SUCCESS) {
        PRINT_ERR("ailiaGetOutputShape failed %d\n", status);
        return;
    }

    PRINT_OUT("output shape %d %d %d %d %d\n", output_shape.x, output_shape.y, output_shape.z, output_shape.w, output_shape.dim);
    int preds_size = output_shape.x*output_shape.y*output_shape.z*output_shape.w*sizeof(float);
    std::vector<float> output_buf =std::vector<float>(preds_size);

    // inference
    status = ailiaPredict(net, &output_buf[0], preds_size, &input_buf[0], input_size);
    if (status != AILIA_STATUS_SUCCESS) {
        PRINT_ERR("ailiaPredict failed %d\n", status);
        return;
    }

    postprocess(rgb, width, height, &output_buf[0], output_shape.x, output_shape.y);

    PRINT_OUT("Program finished successfully.\n");

    ailiaDestroy(net);
}

static int preprocess(float *dst,unsigned int dst_width,unsigned int dst_height,const unsigned char *src,unsigned int src_width,unsigned int src_height)
{
    unsigned char max_value=1;
    for(int y=0;y<src_height;y++){
        for(int x=0;x<src_width;x++){
            for(int rgb=0;rgb<3;rgb++){
                unsigned char value=src[y*src_width*4+x*4+rgb];
                if(max_value<value){
                    max_value=value;
                }
            }
        }
    }
    
    const float mean[3] = {0.485, 0.456, 0.406};
    const float std[3] = {0.229, 0.224, 0.225};

    //src : channel last (bgr order)
    //dst : channel first (bgr order) (u2net)
    //      channel first (rgb order) (midas)

    for(int y=0;y<dst_height;y++){
        for(int x=0;x<dst_width;x++){
            float sx_f=x*src_width/dst_width;
            float sy_f=y*src_height/dst_height;
            
            int sx1=(int)sx_f;
            int sy1=(int)sy_f;
            int sx2=((sx1+1) < (src_width -1)) ? sx1+1:src_width-1;
            int sy2=((sy1+1) < (src_height-1)) ? sy1+1:src_height-1;

            int a=sx_f-sx1;
            int b=sy_f-sy1;
            
            for(int i=0;i<3;i++){
                //bilinear
                unsigned char v1=src[sy1*src_width*4+sx1*4+i];
                unsigned char v2=src[sy1*src_width*4+sx2*4+i];
                unsigned char v3=src[sy2*src_width*4+sx1*4+i];
                unsigned char v4=src[sy2*src_width*4+sx2*4+i];
                unsigned char v=(v1*(1-a)+v2*a)*(1-b)+(v3*(1-a)+v4*a)*b;
#ifdef MIDAS
                dst[y*dst_width+x+(3-1-i)*dst_width*dst_height]=(v/255.0 - mean[i]) / std[i];
#else
                dst[y*dst_width+x+i*dst_width*dst_height]=(1.0*v/max_value - mean[i]) / std[i];
#endif
            }
        }
    }

    return 0;
}

static int postprocess(unsigned char *dst,unsigned int dst_width,unsigned int dst_height,const float *src,unsigned int src_width,unsigned int src_height)
{
    float min = FLT_MAX;
    float max = FLT_MIN;
    for(int y=0;y<src_height;y++){
        for(int x=0;x<src_width;x++){
            float value=src[y*src_width+x];
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
            //bilinear
            float sx_f=x*src_width/dst_width;
            float sy_f=y*src_height/dst_height;
            
            int sx1=(int)sx_f;
            int sy1=(int)sy_f;
            int sx2=((sx1+1) < (src_width -1)) ? sx1+1:src_width-1;
            int sy2=((sy1+1) < (src_height-1)) ? sy1+1:src_height-1;

            int a=sx_f-sx1;
            int b=sy_f-sy1;

            float v1=src[sy1*src_width+sx1];
            float v2=src[sy1*src_width+sx2];
            float v3=src[sy2*src_width+sx1];
            float v4=src[sy2*src_width+sx2];
            float v=(v1*(1-a)+v2*a)*(1-b)+(v3*(1-a)+v4*a)*b;

#ifdef MIDAS
            int out_bit = 8;
            float max_val = pow(2,out_bit)-1;
            float depth_value = max_val * (v - min) / (max - min);

            for(int i=0;i<3;i++){
                dst[y*dst_width*4+x*4+i] = (int)(depth_value);
            }
#else
            float alpha_value = ((v-min) / (max-min));

            for(int i=0;i<3;i++){
                dst[y*dst_width*4+x*4+i] *= alpha_value;
            }
#endif
        }
    }

    return 0;
}

static void releaseData(void *info, const void *data, size_t size)
{
    free((unsigned char *)data);
}

- (void)viewDidLoad {
    [super viewDidLoad];

    // Load image
    UIImage *img = [UIImage imageNamed:@"input.png"];
    //UIImage *img = [UIImage imageNamed:@"input2.jpg"];

    // Get data buffer from loaded image
    CGImageRef image = [img CGImage];
    NSUInteger width = CGImageGetWidth(image);
    NSUInteger height = CGImageGetHeight(image);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    unsigned char *rawData=(unsigned char *)malloc(height * width * 4);
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(&rawData[0], width, height, bitsPerComponent, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    CGContextRelease(context);

    // Predict
    ailia_u2net(&rawData[0],(unsigned int)width,(unsigned int)height);

    // Create new image from data buffer
    size_t bitsPerPixel             = CGImageGetBitsPerPixel(image);

    CGColorSpaceRef colorspace      = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo         = CGImageGetBitmapInfo(image);
    CGDataProviderRef provider      = CGDataProviderCreateWithData(NULL, &rawData[0], height * width * 4, releaseData);
    
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

    // Put image to view
    UIImageView *imageView = [[UIImageView alloc] initWithImage:newImage];

    CGFloat pWidth = [UIScreen mainScreen].bounds.size.width;
    CGFloat pHeight = [UIScreen mainScreen].bounds.size.height;

    [imageView setCenter:CGPointMake(pWidth/2, pHeight/2)];

    [self.view addSubview:imageView];
}


@end
