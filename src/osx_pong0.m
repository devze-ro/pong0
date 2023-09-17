#include <stdio.h>
#include <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong, nonatomic) NSWindow *window;
@end

@implementation AppDelegate
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSRect contentRect = NSMakeRect(0, 0, 640, 480);
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

    NSView *view = [[NSView alloc] initWithFrame:contentRect];
    [self.window setContentView:view];
    [self.window makeKeyAndOrderFront:self];
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

