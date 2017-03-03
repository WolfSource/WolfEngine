#import "ViewController.h"
#import <QuartzCore/CAMetalLayer.h>
#include <MoltenVK/vk_mvk_datatypes.h>

//#include "main_wrapper.h"

#include <w_window.h>
#include "scene.h"
#include <utility>

//global variable to pass NSView to Vulkan c++ code
static NSView*                                      sSampleView;
static CVDisplayLinkRef                             sDisplayLink;
static scene                                        sScene;
static std::map<int, std::vector<w_window_info>>    sWindowInfo;

//called from c++
void init_window(struct w_window_info& pInfo)
{
    sSampleView.bounds = CGRectMake(0, 0, pInfo.width, pInfo.height);
    pInfo.window = (void*)CFBridgingRetain(sSampleView);
}

//called from c++
std::string get_base_data_dir()
{
    return [NSBundle.mainBundle.resourcePath stringByAppendingString: @"/"].UTF8String;
}

@implementation ViewController
{
}

+ (void)Release
{
    if (!sScene.get_is_released())
    {
        sScene.release();
    }
    CVDisplayLinkRelease(sDisplayLink);
}

//since this is a single-view app, initialize Vulkan during view loading.
-(void) viewDidLoad
{
    [super viewDidLoad];
    
    //back the view with a layer created by the makeBackingLayer method.
    self.view.wantsLayer = YES;
    
    //pass the view to the sample code
    sSampleView = self.view;
    
    //run the vulkan sample
    w_window_info _window_info;
    _window_info.width = 800;
    _window_info.height = 600;
    _window_info.window = nullptr;
    _window_info.v_sync_enable = true;
    
    //call init_window from objective-c and get the pointer to the window
    init_window(_window_info);
    sWindowInfo.insert( { 0,{ _window_info } } ) ;
    
    CVDisplayLinkCreateWithActiveCGDisplays(&sDisplayLink);
    CVDisplayLinkSetOutputCallback(sDisplayLink, &DisplayLinkCallback, nullptr);
    CVDisplayLinkStart(sDisplayLink);
}

//Resize the window to fit the size of the content as set by the sample code. */
-(void) viewWillAppear
{
    [super viewWillAppear];
    
    CGSize _size = self.view.bounds.size;
    NSWindow* _window = self.view.window;
    NSRect _frame = [_window contentRectForFrameRect: _window.frame];
    NSRect _new_frame = [_window frameRectForContentRect: NSMakeRect(_frame.origin.x, _frame.origin.y, _size.width, _size.height)];
    [_window setFrame: _new_frame display: YES animate: _window.isVisible];
    [_window center];
}

//Rendering loop callback function for use with a CVDisplayLink.
static CVReturn DisplayLinkCallback(CVDisplayLinkRef pDisplayLink,
                                    const CVTimeStamp* pNow,
                                    const CVTimeStamp* pOutputTime,
                                    CVOptionFlags pFlagsIn,
                                    CVOptionFlags* pFlagsOut,
                                    void* pScene)
{
    sScene.run(sWindowInfo);
    return kCVReturnSuccess;
}

@end


@implementation DemoView

//Indicates that the view wants to draw using the backing layer instead of using drawRect:
-(BOOL) wantsUpdateLayer { return YES; }

//Returns a Metal-compatible layer.
+(Class) layerClass { return [CAMetalLayer class]; }

//If the wantsLayer property is set to YES, this method will be invoked to return a layer instance.
-(CALayer*) makeBackingLayer
{
    CALayer* layer = [self.class.layerClass layer];
    CGSize viewScale = [self convertSizeToBacking: CGSizeMake(1.0, 1.0)];
    layer.contentsScale = MIN(viewScale.width, viewScale.height);
    return layer;
}

@end
