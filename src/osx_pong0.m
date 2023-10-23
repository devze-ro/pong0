#include <stdio.h>
#include <Cocoa/Cocoa.h>
#include "pong0.h"
#include "pong0.c"

#define CVDISPLAYLINK_REFRESH

#ifdef CVDISPLAYLINK_REFRESH
#include <CoreVideo/CoreVideo.h>
#endif

BackBuffer backBuffer;
u32 frameCount = 0;
NSTimeInterval startTime = 0;
NSTimeInterval lastFrameRateCheck = 0;
f32 timePerFrame = 0;

#ifdef CVDISPLAYLINK_REFRESH
CVDisplayLinkRef displayLink;
CVReturn MyDisplayLinkCallback(
        CVDisplayLinkRef displayLink,
        const CVTimeStamp* now,
        const CVTimeStamp* outputTime,
        CVOptionFlags flagsIn,
        CVOptionFlags* flagsOut,
        void* displayLinkContext);
#endif

@interface CustomView : NSView
@end

@implementation CustomView

- (void)drawRect:(NSRect)dirtyRect {
    CGContextRef context = [NSGraphicsContext currentContext].CGContext;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

    CGContextRef bufferContext = CGBitmapContextCreate(backBuffer.memory,
            backBuffer.width,
            backBuffer.height,
            8,
            backBuffer.pitch,
            colorSpace,
            kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host);

    CGImageRef image = CGBitmapContextCreateImage(bufferContext);
    CGContextDrawImage(context, CGRectMake(0, 0, backBuffer.width, backBuffer.height), image);

    CGImageRelease(image);
    CGContextRelease(bufferContext);
    CGColorSpaceRelease(colorSpace);
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong, nonatomic) NSWindow *window;
@end

@implementation AppDelegate
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSRect contentRect = NSMakeRect(0, 0, 960, 720);
    self.window = [[NSWindow alloc] initWithContentRect:contentRect styleMask:(
            NSWindowStyleMaskTitled |
            NSWindowStyleMaskClosable)
        backing:NSBackingStoreBuffered
          defer:NO];

    [self.window setTitle:@"pong0"];
    // TODO (devze_ro): The following code makes sure that the game window is
    // always topmost window. This is added for easier debugging and needs to
    // be removed or enclosed inside a debug flag.
    [self.window setLevel:NSFloatingWindowLevel];
    [self.window center];

    CustomView *view = [[CustomView alloc] initWithFrame:contentRect];
    [self.window setContentView:view];
    [self.window makeKeyAndOrderFront:self];

    v2u window_dimensions = (v2u){0};
    window_dimensions.x = 960;
    window_dimensions.y = 720;
    init_game(&backBuffer, &window_dimensions);

#ifdef CVDISPLAYLINK_REFRESH
    printf("Using CVDisplayLink to refresh!\n");

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, MyDisplayLinkCallback, (__bridge void *)(self));
    CVDisplayLinkStart(displayLink);

    CVTime refreshPeriod = CVDisplayLinkGetNominalOutputVideoRefreshPeriod(displayLink);
    if (refreshPeriod.flags & kCVTimeIsIndefinite) {
        NSLog(@"Refresh rate is indefinite");
    } else {
        timePerFrame = refreshPeriod.timeValue / (double)refreshPeriod.timeScale;

        NSLog(@"time per frame: %f sec", timePerFrame);
    }

#else
    printf("Using timer to refresh!\n");

    [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
                                     target:self
                                   selector:@selector(timerFired:)
                                   userInfo:nil
                                    repeats:YES];
#endif
}

#ifdef CVDISPLAYLINK_REFRESH
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext) {
    AppDelegate *self = (__bridge AppDelegate *)displayLinkContext;
    update_game(&backBuffer, timePerFrame);
    CustomView *view = (CustomView *)self.window.contentView;
    dispatch_async(dispatch_get_main_queue(), ^{
        [view setNeedsDisplay:YES];
    });

    [self measureFrameRate];

    return kCVReturnSuccess;
}
#else
- (void)timerFired:(NSTimer *)timer {
    // Your color changing and drawing code here.
    update_game(&backBuffer, 1.0 / 60.0);
    CustomView *view = (CustomView *)self.window.contentView;
    [view setNeedsDisplay:YES];
    [self measureFrameRate];
}
#endif

- (void)measureFrameRate {
    frameCount++;
    
    if (startTime == 0) {
        startTime = [NSDate timeIntervalSinceReferenceDate];
    }
    
    NSTimeInterval currentTime = [NSDate timeIntervalSinceReferenceDate];
    NSTimeInterval elapsed = currentTime - startTime;
    
    if (elapsed >= 1.0) {
        if (!timePerFrame) {
            timePerFrame = elapsed / frameCount;
        }
        f32 frameRate = frameCount / elapsed;
        NSLog(@"Current frame rate: %f fps", frameRate);
        
        frameCount = 0;
        startTime = currentTime;
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {}
@end

int main(int argc, const char * argv[]) {
    printf("Welcome to Pong0!\n");

    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        AppDelegate *appDelegate = [[AppDelegate alloc] init];

        [app setDelegate:appDelegate];
        [app run];
    }

    return 0;
}

