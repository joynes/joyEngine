//
//  ViewController.m
//  joyEngine
//
//  Created by Johannes Kählare on 31/03/2018.
//  Copyright © 2018 Johannes Kählare. All rights reserved.
//

#import "ViewController.h"
#import <GLKit/GLKit.h>
#import <OpenGLES/ES2/glext.h>
#include "shared.h"

@interface ViewController ()
@property (strong, nonatomic) EAGLContext *context;

- (void)tearDownGL;

@end

@implementation ViewController


- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    NSLog(@"%d", UIScreen.mainScreen.maximumFramesPerSecond);
    update_game(rect.size.width, rect.size.height);
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    self.preferredFramesPerSecond = UIScreen.mainScreen.maximumFramesPerSecond;
    
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [EAGLContext setCurrentContext:self.context];
    setup(0, UIScreen.mainScreen.maximumFramesPerSecond);
}

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}



@end

