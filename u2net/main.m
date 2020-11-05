//
//  main.m
//  u2net
//
//  Created by ax Inc.on 2020/11/05.
//  Copyright © 2020 ax Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    NSString * appDelegateClassName;
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
        appDelegateClassName = NSStringFromClass([AppDelegate class]);
    }
   
    return UIApplicationMain(argc, argv, nil, appDelegateClassName);
}
