#import "ViewController.h"

@implementation ViewController
{
}

+ (void)Release
{
    //Release
}

//since this is a single-view app, initialize Vulkan during view loading.
-(void) viewDidLoad
{
    [super viewDidLoad];
    
    //back the view with a layer created by the makeBackingLayer method.
    self.view.wantsLayer = YES;
}

//Resize the window to fit the size of the content as set by the sample code.
-(void) viewWillAppear
{
    [super viewWillAppear];
}

@end
