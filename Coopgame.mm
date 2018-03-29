#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
#import <GLKit/GLKit.h>
#import "shared.h"

@interface GameView: NSOpenGLView {
  CVDisplayLinkRef displayLink;
}
@end

@implementation GameView
- (void)prepareOpenGL {
  
  // Synchronize buffer swaps with vertical refresh rate
  GLint swapInt = 1;
  [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

  // Create a display link capable of being used with all active displays
  CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

  // Set the renderer output callback function
  CVDisplayLinkSetOutputCallback(displayLink, &DisplayLinkCallback, self);

  // Set the display link for the current renderer
  CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
  CGLPixelFormatObj cglPixelFormat = static_cast<CGLPixelFormatObj>([self createPixelFormat]);
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

  // Activate the display link
  CVDisplayLinkStart(displayLink);
  setup(22, 60); // frag offset
}

- (NSOpenGLPixelFormat*)createPixelFormat
{
  NSOpenGLPixelFormat *pixelFormat;

  NSOpenGLPixelFormatAttribute attribs[] =
  {
    NSOpenGLPFANoRecovery,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAColorSize,        32,
    NSOpenGLPFAAlphaSize,        8,
    NSOpenGLPFAScreenMask,
    CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
    0
  };

  pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
  return [pixelFormat autorelease];
}

static CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime,
CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    CVReturn result = [(GameView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    // Add your drawing codes here
  NSOpenGLContext *currentContext = [self openGLContext];
  [currentContext makeCurrentContext];

  // must lock GL context because display link is threaded
  CGLLockContext(static_cast<CGLContextObj>([currentContext CGLContextObj]));
  // Add your drawing codes here
  update_game(WIDTH, HEIGHT);

  [currentContext flushBuffer];

  CGLUnlockContext(static_cast<CGLContextObj>([currentContext CGLContextObj]));
  return kCVReturnSuccess;
}

- (void)dealloc
{
    // Release the display link
    CVDisplayLinkRelease(displayLink);
    [super dealloc];
}
@end

int main() {
  [NSAutoreleasePool new];
  [NSApplication sharedApplication];
  [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

  id window = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, WIDTH, HEIGHT) styleMask:NSWindowStyleMaskTitled backing:NSBackingStoreBuffered defer:NO] autorelease];
  [window setTitle:@"Game"];
  [window makeKeyAndOrderFront:nil];
  [NSApp activateIgnoringOtherApps:YES];

  GameView *view = [[GameView alloc] initWithFrame:NSMakeRect(0, 0, WIDTH, HEIGHT)];
  [view setWantsLayer:YES];
  [[window contentView] addSubview:view];
  [NSApp run];
}
