//
//  ViewController.m
//  joyEngine
//
//  Created by Johannes Kählare on 01/04/2018.
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
    
    float isf = 1.0 / [UIScreen mainScreen].scale;
    
    update_game(rect.size.width / isf, rect.size.height / isf);
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
    setup(0, (int)UIScreen.mainScreen.maximumFramesPerSecond);
    
    UISwipeGestureRecognizer *swipeRight = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeFrom:)];
    [self.view addGestureRecognizer:swipeRight];
    
    UISwipeGestureRecognizer *swipeLeft = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeFrom:)];
    [swipeLeft setDirection:(UISwipeGestureRecognizerDirectionLeft)];
    [self.view addGestureRecognizer:swipeLeft];
    
    UISwipeGestureRecognizer *swipeUp = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeFrom:)];
    [swipeUp setDirection:(UISwipeGestureRecognizerDirectionUp)];
    [self.view addGestureRecognizer:swipeUp];
    
    UISwipeGestureRecognizer *swipeDown = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeFrom:)];
    [swipeDown setDirection:(UISwipeGestureRecognizerDirectionDown)];
    [self.view addGestureRecognizer:swipeDown];
    
}

-(void)handleSwipeFrom:(UISwipeGestureRecognizer *)sender {
    if (sender.direction == UISwipeGestureRecognizerDirectionLeft) {
        move_left();
    } else if (sender.direction == UISwipeGestureRecognizerDirectionRight) {
        move_right();
    } else if (sender.direction == UISwipeGestureRecognizerDirectionUp) {
        move_up();
    } else if (sender.direction == UISwipeGestureRecognizerDirectionDown) {
        move_down();
    }
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


