#ifdef __APPLE__
#import <Foundation/Foundation.h>
#import <Foundation/NSProcessInfo.h>
void __macos__disable_app_nap(void) {
   if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]){
      [[NSProcessInfo processInfo] beginActivityWithOptions:0x00FFFFFF reason:@"No, I don't want to nap!"];
   }
}
#endif // __APPLE__