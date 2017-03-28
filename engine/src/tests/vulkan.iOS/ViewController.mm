#import "ViewController.h"
#import <QuartzCore/CAMetalLayer.h>
#include <MoltenVK/mvk_datatypes.h>

#include <w_window.h>
#include "scene.h"
#include <utility>

static UIView*                                      sSampleView;
static CADisplayLink*                               sDisplayLink;
static scene*                                       sScene;
static std::map<int, std::vector<w_window_info>>    sWindowInfo;

@implementation ViewController
{
   
}

+ (void)Release
{
    if (sScene && !sScene->get_is_released())
    {
        sScene->release();
    }
}


//Since this is a single-view app, init Vulkan when the view is loaded.
-(void) viewDidLoad
{
    [super viewDidLoad];
    
    self.view.contentScaleFactor = UIScreen.mainScreen.nativeScale;
    
    //pass the view to the sample code
    sSampleView = self.view;
    
    sScene = new scene([NSBundle.mainBundle.resourcePath stringByAppendingString: @"/"].UTF8String);
    
    //run the vulkan sample
    w_window_info _window_info;
    _window_info.width = self.view.bounds.size.width;
    _window_info.height = self.view.bounds.size.height;
    _window_info.window = (void*)CFBridgingRetain(sSampleView);
   
    //call init_window from objective-c and get the pointer to the window
    //init_window(_window_info);
    sWindowInfo.insert( { 0,{ _window_info } } ) ;
    
    
    uint32_t fps = 60;
    sDisplayLink = [CADisplayLink displayLinkWithTarget: self selector: @selector(on_render)];
    [sDisplayLink setFrameInterval: 60 / fps];
    [sDisplayLink addToRunLoop: NSRunLoop.currentRunLoop forMode: NSDefaultRunLoopMode];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void) on_render
{
    //printf("H");
    sScene->run(sWindowInfo);
}

@end

@implementation DemoView

/** Returns a Metal-compatible layer. */
+(Class) layerClass { return [CAMetalLayer class]; }

@end
